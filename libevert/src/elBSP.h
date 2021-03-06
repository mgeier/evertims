#ifndef __ELBSP_HPP
#define __ELBSP_HPP
/*************************************************************************
 *
 * This file is part of the EVERT beam-tracing Library
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
 * (C) 2004-2005 Samuli Laine
 * Helsinki University of Technology
 *
 * (C) 2008-2015 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ************************************************************************/

#if !defined (__ELAABB_HPP)
    #include "elAABB.h"
#endif
#if !defined (__ELVECTOR_HPP)
    #include "elVector.h"
#endif

namespace EL
{
    
    //------------------------------------------------------------------------
    
    class Beam;
    class Polygon;
    class Ray;
    
    class BSP
    {
        
    public:
        
        BSP (void);
        ~BSP (void);
        
        void constructHierarchy (const Polygon** polygons, int numPolygons);
        
        void beamCast (const Beam& beam, std::vector<const Polygon*>& result) const;
        const Polygon* rayCast (const Ray& ray) const;
        bool rayCastAny (const Ray& ray) const;
        static Vector3 getIntersectionPoint (void);
        
        class TempNode;
        
        
    private:
        
        TempNode* m_hierarchy;
        uintptr_t* m_list;
        AABB m_aabb;
    };
    
    //------------------------------------------------------------------------
} // namespace EL

#endif // __ELBSP_HPP
