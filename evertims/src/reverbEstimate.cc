/***********************************************************************
 *
 * This file is part of the EVERTims room acoustic modeling software.
 * It is released under the MIT License. You should have received a
 * copy of the MIT License along with EVERTims.  If not, see
 * http://www.opensource.org/licenses/mit-license.php
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * For details, see the LICENSE file
 *
 * The Reverberation estimation class implementation for EVERTims
 *
 * (C) 2009 Lauri Savioja
 * Helsinki University of Technology
 *
 * (C) 2007-2015 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ***********************************************************************/

#include <iostream>
#include <fstream>

#include "reverbEstimate.h"
#include "elPolygon.h"
#include "utils.h"

#define SPEED_OF_SOUND 340

Response::Response(double samplingFrequency, double lengthInSeconds):
m_samplingFrequency(samplingFrequency),
m_length((int)(lengthInSeconds * samplingFrequency))
{
    m_signal = new double[m_length];
    // DPQ: set original values to zero since used recursively afterwards
    for( int i = 0; i < m_length; i++){ m_signal[i] = 0.0; }
}

Response::~Response()
{
    delete m_signal;
}

void Response::setItem(double time, double value)
{
    int idx = time * m_samplingFrequency;
    if (idx < m_length){ m_signal[idx] = value; }
}

void Response::addItem(double time, double value)
{
    int idx = time * m_samplingFrequency;
    if (idx < m_length){ m_signal[idx] += value; }
}

void Response::SchroederIntegrate()
{
    int idx = m_length;
    double sum = 0;
    
    while (idx)
    {
        sum += m_signal[idx];
        m_signal[idx--] = sum;
    }
    
    idx = m_length;
    while (idx)
    {
        // DPQ: avoid adding -Inf values to m_signal when (guess here:) processing solution
        if (m_signal[idx] != 0)
        {
            m_signal[idx] = 10 * log10(m_signal[idx] / sum);
        }
        
        idx--;
    }
}

float Response::search (double val)
{
    int idx = 0;
    
    for (; idx < m_length; idx++){ if (m_signal[idx] <= val){ break; }}
    
    return (float)idx / m_samplingFrequency;
}

void Response::getMaxMin(double& maxValue, double &maxTime, double& minValue, double& minTime)
{
    int idx;
    
    for (idx = 1; idx < m_length; idx++)
    {
        if ( m_signal[idx] != m_signal[idx-1] ){ break; }
    }
    
    if (idx < m_length)
    {
        maxValue = m_signal[idx];
        maxTime  = (double)idx / m_samplingFrequency;
    }
    
    for (idx = m_length - 1; idx >= 0; idx--)
    {
        if ( m_signal[idx] != m_signal[idx+1] ){ break; }
    }
    
    if (idx >= 0)
    {
        minValue = m_signal[idx];
        minTime  = (double)idx / m_samplingFrequency;
    }
}

void Response::print(char *name)
{
    std::ofstream outs;
    
    outs.open (name);
    
    for (int i = 0; i < m_length; i++)
    {
        outs << m_signal[i] << std::endl;
    }
    
    outs.close();
}

ReverbEstimator::ReverbEstimator (double samplingFrequency, EL::PathSolution *solution, double speedOfSound, double maxTime)
{
    int idx = 0;
    
    for (int i = 0; i < MAX_BANDS; i++)
    {
        m_SchroederPlots[i] = new Response(samplingFrequency, maxTime);
    }
    
    double len;
    double time;
    float reflectance[MAX_BANDS];
    
    for (int i = 0; i < solution->numPaths(); i++)
    {
        const EL::PathSolution::Path& path = solution->getPath(i);
        
        len = solution->getLength( path );
        time = len / speedOfSound;
        
        for (int k = 0; k < MAX_BANDS; k++){ reflectance[k] = 1.0; }
        
        for (int j = 0; j < path.m_order; j++)
        {
            const EL::Polygon* p = path.m_polygons[j];
            const Material& m = p->getMaterial ();
            for (int k = 0; k < MAX_BANDS; k++) reflectance[k] *= ( 1 - m.absorption[k] );
        }
        
        for (int k = 0; k < MAX_BANDS; k++)
        {
            m_SchroederPlots[k]->addItem(time, reflectance[k]);  // Should I divide this by len ?
        }
    }
    //  m_SchroederPlots[0]->print("energy.txt");
    
    for (int k = 0; k < MAX_BANDS; k++)
    {
        m_SchroederPlots[k]->SchroederIntegrate();
    }
    
    //  m_SchroederPlots[0]->print("schroeder.txt");
}


ReverbEstimator::~ReverbEstimator()
{
    for (int i = 0; i < MAX_BANDS; i++){ delete m_SchroederPlots[i]; }
}

float ReverbEstimator::getEstimateR60(int band, double startDecay, double endDecay)
{
    Response* r = m_SchroederPlots[band];
    
    COUT << "Starting to search the region: " << startDecay << " - " << endDecay << "\n";
    
    float realStart = r->search();
    COUT << "realStart = " << realStart << "\n";
    
    float startTime = r->search(startDecay);
    COUT << "startTime = " << startTime << "\n";
    float endTime   = r->search(endDecay);
    COUT << "endTime = " << endTime << "\n";
    
    float totalDecay = realStart + (60 * (endTime-startTime)/(endDecay - startDecay));
    
    return totalDecay;
}

void ReverbEstimator::getMaxMin(int band, double& maxValue, double& maxTime, double& minValue, double& minTime)
{
    Response* r = m_SchroederPlots[band];
    
    r->getMaxMin(maxValue, maxTime, minValue, minTime);
}

