//---------------------------------------------------------------------------//
// Copyright (c) 2013-2014 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#include <boost/compute/config.hpp>

#define BOOST_TEST_MODULE TestStruct
#include <boost/test/unit_test.hpp>

#include <boost/compute/function.hpp>
#include <boost/compute/algorithm/find_if.hpp>
#include <boost/compute/algorithm/transform.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/functional/field.hpp>
#include <boost/compute/types/struct.hpp>

namespace compute = boost::compute;

// example code defining an atom class
namespace chemistry {

struct Atom
{
    Atom(float _x, float _y, float _z, int _number)
        : x(_x), y(_y), z(_z), number(_number)
    {
    }

    float x;
    float y;
    float z;
    int number;
};

} // end chemistry namespace

// adapt the chemistry::Atom class
BOOST_COMPUTE_ADAPT_STRUCT(chemistry::Atom, Atom, (x, y, z, number))

#include "check_macros.hpp"
#include "context_setup.hpp"

BOOST_AUTO_TEST_CASE(atom_type_name)
{
    BOOST_CHECK(std::strcmp(compute::type_name<chemistry::Atom>(), "Atom") == 0);
}

BOOST_AUTO_TEST_CASE(atom_struct)
{
    std::vector<chemistry::Atom> atoms;
    atoms.push_back(chemistry::Atom(1.f, 0.f, 0.f, 1));
    atoms.push_back(chemistry::Atom(0.f, 1.f, 0.f, 1));
    atoms.push_back(chemistry::Atom(0.f, 0.f, 0.f, 8));

    compute::vector<chemistry::Atom> vec(atoms.size(), context);
    compute::copy(atoms.begin(), atoms.end(), vec.begin(), queue);

    // find the oxygen atom
    BOOST_COMPUTE_FUNCTION(bool, is_oxygen, (chemistry::Atom),
    {
        return _1.number == 8;
    });

    compute::vector<chemistry::Atom>::iterator iter =
        compute::find_if(vec.begin(), vec.end(), is_oxygen, queue);
    BOOST_CHECK(iter == vec.begin() + 2);

    // copy the atomic numbers to another vector
    compute::vector<int> atomic_numbers(vec.size(), context);
    compute::transform(
        vec.begin(), vec.end(),
        atomic_numbers.begin(),
        compute::field<int>("number"),
        queue
    );
    CHECK_RANGE_EQUAL(int, 3, atomic_numbers, (1, 1, 8));
}

BOOST_AUTO_TEST_SUITE_END()
