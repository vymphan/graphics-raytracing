#include "shape.h"

#include "debugging.h"

#include <vector>
#include <cmath> // std::sqrt()

using namespace glm;

namespace graphics101 {

Intersection Plane::rayIntersect( const ray3& ray ) const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose( transformInverse() )` times the object-space normal.

    vec3 p = transformInverse() * vec4( ray.p, 1 );
    vec3 d = transformInverse() * vec4( ray.d, 0 );
    vec3 n = vec3( 0, 0, 1 );

    // F(x,y,z) = z

    real t = - p[2] / ( 1.0 * d[2] );

    if ( t >= 0 ) {
        result.valid = true;
        result.t = t;
        result.normal = transpose( transformInverse() ) * vec4( n , 0 );
        result.material = material();
        result.position = ray.p + t * ray.d;
    }

    return result;
}

Intersection Sphere::rayIntersect( const ray3& ray ) const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose( transformInverse() )` times the object-space normal.
    vec3 p = transformInverse() * vec4( ray.p, 1 );
    vec3 d = transformInverse() * vec4( ray.d, 0 );

    //a = dx^2 + dy^2 + dz^2
    //b = 2*( px*dx + py*dy + pz*dz )
    //c = px^2 + py^2 + pz^2 - 1.0

    real a = pow( d[0], 2 ) + pow( d[1], 2 ) + pow( d[2], 2 );
    real b = 2.0 * ( p[0]*d[0] + p[1]*d[1] + p[2]*d[2] );
    real c = pow( p[0], 2 ) + pow( p[1], 2 ) + pow( p[2], 2 ) - 1.0;

    std::vector< Intersection > ts;
    real t;
    vec3 q, n;

    real radical = b * b - 4. * a * c;
    if ( radical >= 0 ) {
        Intersection i;
        t = ( - b + sqrt( radical ) )/( 2. * a );
        if ( t >= 0 ) {
            q = p + t * d;
            n = vec3( 2*q[0], 2*q[1], 2*q[2] );

            i.valid = true;
            i.t = t;
            i.position = ray.p + t * ray.d;
            i.normal = transpose( transformInverse() ) * vec4( n, 0 );
            i.material = material();
            ts.push_back( i );
        }

        t = ( - b - sqrt( radical ) )/( 2. * a );
        if ( t >= 0 ) {
            q = p + t * d;
            n = vec3( 2*q[0], 2*q[1], 2*q[2] );

            i.valid = true;
            i.t = t;
            i.position = ray.p + t * ray.d;
            i.normal = transpose( transformInverse() ) * vec4( n, 0 );
            i.material = material();
            ts.push_back( i );
        }
    }

    if ( !ts.empty() ) {
        for ( const Intersection& itm : ts ) {
            if ( itm.t < result.t ) {
                result = itm;
            }
        }
    }
    return result;
}

Intersection Cylinder::rayIntersect( const ray3& ray ) const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose( transformInverse() )` times the object-space normal.

    vec3 p = transformInverse() * vec4( ray.p, 1 );
    vec3 d = transformInverse() * vec4( ray.d, 0 );

    std::vector< Intersection > ts;
    real t;
    vec3 q, n;
    Intersection i;

    // Top and Bottom of Cylinder:
    // The bottom face F(x,y,z) = -z
    t = -p[2]/( 1.0 * d[2] );
    if ( t >= 0 ) {
        q = p + t * d;
        if ( ( pow( q[0], 2 ) + pow( q[1], 2 ) ) < 1 ) {
            i.valid = true;
            i.t = t;
            i.position = ray.p + t*ray.d;
            i.normal = transpose( transformInverse() ) * vec4( 0, 0, -1, 0 );
            i.material = material();
            if ( pow( q[0], 2 ) + pow( q[1], 2 ) < 1 ) {
                ts.push_back( i );
            }
        }
    }

    //For the top face F(x,y,z) = z - 1
    t = ( 1. - p[2] )/( 1. * d[2] );
    if ( t >= 0 ) {
        q = p + t * d;
        if ( ( pow( q[0], 2 ) + pow( q[1], 2 ) ) < 1 ) {
            i.valid = true;
            i.t = t;
            i.position = ray.p + t*ray.d;
            i.normal = transpose( transformInverse() ) * vec4( 0, 0, 1, 0 );
            i.material = material();
            if ( pow( q[0], 2 ) + pow( q[1], 2 ) < 1 ) {
                ts.push_back( i );
            }
        }
    }

    // Side of Cylinder
    //a = dx^2 + dy^2
    //b = 2*( px*dx + py*dy )
    //c = px^2 + py^2 - 1.0

    real a = pow( d[0], 2 ) + pow( d[1], 2 );
    real b = 2.0 * ( p[0]*d[0] + p[1]*d[1] );
    real c = pow( p[0], 2 ) + pow( p[1], 2 ) - 1.0;


    real radical = b * b - 4. * a * c;

    if ( radical >= 0 ) {
        t = ( - b + sqrt( radical ) )/( 2. * a );
        if ( t >= 0 ) {
            q = p + t * d;
            n = vec3( 2*q[0], 2*q[1], 0 );

            i.valid = true;
            i.t = t;
            i.position = ray.p + t * ray.d;
            i.normal = transpose( transformInverse() ) * vec4( n, 0 );
            i.material = material();
            if ( q[2] > 0 && q[2] < 1 ) {
                ts.push_back( i );
            }
        }

        t = ( - b - sqrt( radical ) )/( 2. * a );
        if ( t >= 0 ) {
            q = p + t * d;
            n = vec3( 2.*q[0], 2.*q[1], 0 );

            i.valid = true;
            i.t = t;
            i.position = ray.p + t * ray.d;
            i.normal = transpose( transformInverse() ) * vec4( n, 0 );
            i.material = material();
            if ( q[2] > 0 && q[2] < 1 ) {
                ts.push_back( i );
            }
        }
    }

    if ( !ts.empty() ) {
        for ( const Intersection& itm : ts ) {
            if ( itm.t < result.t ) {
                result = itm;
            }
        }
    }
    return result;
}

Intersection Cone::rayIntersect( const ray3& ray ) const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose( transformInverse() )` times the object-space normal.

    vec3 p = transformInverse() * vec4( ray.p, 1 );
    vec3 d = transformInverse() * vec4( ray.d, 0 );

    std::vector< Intersection > ts;
    real t;
    vec3 q, n;
    Intersection i;

    // The bottom face F(x,y,z) = -z
    t = -p[2]/( 1. * d[2]);
    if ( t >= 0 ) {
        q = p + t * d;
        if ( ( pow( q[0], 2 ) + pow( q[1], 2 ) ) < 1 ) {
            i.valid = true;
            i.t = t;
            i.position = ray.p + t*ray.d;
            i.normal = transpose( transformInverse() ) * vec4( 0, 0, -1, 0 );
            i.material = material();
            if ( pow( q[0], 2 ) + pow( q[1], 2 ) < 1 ) {
                ts.push_back( i );
            }
        }
    }



    //a = dx^2 + dy^2 - dz^2
    //b = 2*( px*dx + py*dy + (1-pz)*dz )
    //c = px^2 + py^2 - (1-pz)^2

    real a = pow( d[0], 2 ) + pow( d[1], 2 ) - pow( d[2], 2 );
    real b = 2.0 * ( p[0]*d[0] + p[1]*d[1] + (1 - p[2])*d[2] );
    real c = pow( p[0], 2 ) + pow( p[1], 2 ) - pow((1.0 - p[2]), 2);

    real radical = b * b - 4. * a * c;

    if ( radical >= 0. ) {
        t = ( - b + sqrt( radical ) )/( 2. * a );
        if ( t >= 0 ) {
            q = p + t * d;
            n = vec3( 2*q[0], 2*q[1], 2*( 1 - q[2] ) );

            i.valid = true;
            i.t = t;
            i.position = ray.p + t * ray.d;
            i.normal = transpose( transformInverse() ) * vec4( n, 0 );
            i.material = material();
            if ( q[2] > 0 && q[2] <= 1 ) {
                ts.push_back( i );
            }
        }

        t = ( - b - sqrt( radical ) )/( 2. * a );
        if ( t >= 0 ) {
            q = p + t * d;
            n = vec3( 2*q[0], 2*q[1], 2*( 1 - q[2] ) );

            i.valid = true;
            i.t = t;
            i.position = ray.p + t * ray.d;
            i.normal = transpose( transformInverse() ) * vec4( n, 0 );
            i.material = material();
            if ( q[2] > 0 && q[2] <= 1 ) {
                ts.push_back( i );
            }
        }
    }

    if ( !ts.empty() ) {
        for ( const Intersection& itm : ts ) {
            if ( itm.t < result.t ) {
                result = itm;
            }
        }
    }
    return result;
}

Intersection Cube::rayIntersect( const ray3& ray ) const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose( transformInverse() )` times the object-space normal.

    vec3 p = transformInverse() * vec4( ray.p, 1. );
    vec3 d = transformInverse() * vec4( ray.d, 0. );

    std::vector< Intersection > ts;
    real t;
    vec3 q, n;
    Intersection i;

    //Face 1: if -1 ≤ y,z ≤ 1: F(x,y,z) = x-1
    t = ( 1. - p[0] ) / ( 1. * d[0] ) ;
    if ( t >= 0 ) {
        q = p + t * d;
        n = vec3( 1, 0, 0 );

        i.valid = true;
        i.t = t;
        i.position = ray.p + t * ray.d;
        i.normal = transpose( transformInverse() ) * vec4( n, 0 );
        i.material = material();
        if ( q[1] >= -1 &&  q[2]>= -1 && q[1] <= 1 && q[2] <= 1) {
            ts.push_back( i );
        }
    }


    //Face 2: if -1 ≤ y,z ≤ 1: F(x,y,z) = -(x+1)
    t = - ( 1. + p[0] ) / ( 1. * d[0] );
    if ( t >= 0 ) {
        q = p + t * d;
        n = vec3( -1, 0, 0 );

        i.valid = true;
        i.t = t;
        i.position = ray.p + t * ray.d;
        i.normal = transpose( transformInverse() ) * vec4( n, 0 );
        i.material = material();
        if ( q[1] >= -1 && q[2] >= -1 && q[1] <= 1 && q[2] <= 1 ) {
            ts.push_back( i );
        }
    }

    //Face 3: if -1 ≤ x,z ≤ 1: F(x,y,z) = y-1
    t = ( 1 - p[1] ) / ( 1. * d[1] );
    if ( t >= 0 ) {
        q = p + t * d;
        n = vec3( 0, 1, 0 );

        i.valid = true;
        i.t = t;
        i.position = ray.p + t * ray.d;
        i.normal = transpose( transformInverse() ) * vec4( n, 0 );
        i.material = material();
        if ( q[0] >= -1 && q[2] >= -1 && q[0] <= 1 && q[2] <= 1 ) {
            ts.push_back( i );
        }
    }

    //Face 4: if -1 ≤ x,z ≤ 1: F(x,y,z) = -(y+1)
    t = - ( 1 + p[1] ) / ( 1. * d[1] );
    if ( t >= 0. ) {
        q = p + t * d;
        n = vec3( 0, -1, 0 );

        i.valid = true;
        i.t = t;
        i.position = ray.p + t * ray.d;
        i.normal = transpose( transformInverse() ) * vec4( n, 0 );
        i.material = material();
        if ( q[0] >= -1 && q[2] >= -1 && q[0] <= 1 && q[2] <= 1 ) {
            ts.push_back( i );
        }
    }

    //Face 5: if -1 ≤ x,y ≤ 1: F(x,y,z) = z-1
    t = ( 1 - p[2] ) / ( 1. * d[2] );
    if ( t >= 0 ) {
        q = p + t * d;
        n = vec3( 0, 0, 1 );

        i.valid = true;
        i.t = t;
        i.position = ray.p + t * ray.d;
        i.normal = transpose( transformInverse() ) * vec4( n, 0 );
        i.material = material();
        if ( q[0] >= -1 && q[1] >= -1 && q[0] <= 1 && q[1] <= 1 ) {
            ts.push_back( i );
        }
    }

    //Face 6if -1 ≤ x,y ≤ 1: F(x,y,z) = -(z+1)
    t = - ( 1 + p[2] ) / ( 1. * d[2] );
    if ( t >= 0 ) {
        q = p + t * d;
        n = vec3( 0, 0, -1 );

        i.valid = true;
        i.t = t;
        i.position = ray.p + t * ray.d;
        i.normal = transpose( transformInverse() ) * vec4( n, 0 );
        i.material = material();
        if ( q[0] >= -1 && q[1] >= -1 && q[0] <= 1 && q[1] <= 1 ) {
            ts.push_back( i );
        }
    }

    if ( !ts.empty() ) {
        for ( const Intersection& itm : ts ) {
            if ( itm.t < result.t ) {
                result = itm;
            }
        }
    }
    return result;
}

Intersection Mesh::rayIntersect( const ray3& ray ) const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose( transformInverse() )` times the object-space normal.

    return result;
}

}
