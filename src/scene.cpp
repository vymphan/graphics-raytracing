#include "scene.h"
#include "debugging.h"

// Include all of glm here.
#include "glm/glm.hpp"

using namespace glm;

namespace graphics101 {

void Scene::render( Image& into_image ) {
    // Your code goes here.

    // Iterate over the pixels of the image. For each pixel:
    // 1. Use camera->getPixelUV() and camera->getRay() to create a ray3.
    // 2. Call rayColor() to get a vec3 color for that ray.
    // 3. Use into_image.pixel() to set the pixel color.
    for ( int x = 0; x < into_image.width(); x++ ) {
        for ( int y = 0; y < into_image.height(); y++ ) {
            vec2 uv = camera->getPixelUV( vec2( x, y ), into_image.width(), into_image.height() );
            vec3 c = rayColor( camera->getRay( uv ), 3 );
            c = clamp( round( c*255. ), 0., 255. );
            into_image.pixel( x, y ) = ColorRGBA8( c.r, c.g, c.b );
        }
    }

}

Intersection Scene::closestIntersection( const ray3& ray ) const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.
    
    // Iterate over all the shapes, calling rayIntersect() on each.
    // In C++, a nice iterator for loop can be written as:
    // for( const ShapePtr& shape: shapes ) { ... shape->rayIntersect( ... ) ... }

    for ( const auto& shape : shapes ) {
        const Intersection i = shape->rayIntersect( ray );
        if ( i.valid && i.t < result.t) {
            result = i;
        }
    }
    
    return result;
}
vec3 Scene::rayColor( const ray3& ray, int max_recursion ) const {
    // Your code goes here.

    // In this ray casting assignment, you will simply call closestIntersection(),
    // and, if there is one, return the .material.color_diffuse;
    // Otherwise, return black (0,0,0).

    assert( max_recursion >= 0 );

    const real eps = 1e-7;

    vec3 c( 0,0,0 );

    const Intersection i = closestIntersection( ray );

    if ( !i.valid ) return c;


    // Reflections:
   if ( i.material.reflective && max_recursion > 0 ) {
       const vec3 reflect_direction = glm::reflect( ray.d, glm::normalize( i.normal ) );
       const ray3 reflect_ray = ray3( i.position + eps * reflect_direction, reflect_direction );
       c += i.material.color_reflect * rayColor( reflect_ray, max_recursion - 1 );
   }

    // Recursively getting through every light
    for ( const Light& l : lights ) {
        const vec3 L = glm::normalize( l.position - i.position );
        const vec3 N = glm::normalize( i.normal );
        const vec3 V = glm::normalize( ray.p - i.position );
        const vec3 R = glm::normalize( glm::reflect( (-1.) * L, N ) );
        const real n = i.material.shininess;

        const real dotNL = dot( N, L );

        // Ambient lighting:
        c += i.material.color_ambient * l.color_ambient;

       // Shadow:
       real S = 1.;
       const Intersection& block = closestIntersection( ray3( i.position + eps * L, L ) );
       if ( block.valid && ( block.t < glm::distance( i.position, l.position ) ) ) {
           S = 0.;
       }

        if ( dotNL >= 0 && S != 0 ) {
            // Diffuse lighting:
            c += i.material.color_diffuse * l.color * dotNL;
            // Specular lighting:
            if ( dotNL != 0 ) {
                c += i.material.color_specular * l.color * pow( max( 0., dot( V, R ) ), n );
            }
        }
    }

    return c;
}

}
