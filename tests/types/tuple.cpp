/*
 * Copyright (C) 2024 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <flatmemory/types/tuple.hpp>
#include <flatmemory/types/vector.hpp>

#include <gtest/gtest.h>

#include <string>


namespace flatmemory::tests
{
    TEST(FlatmemoryTests, TypesTupleEmptyTest) {
        // Test empty tuple
        EXPECT_EQ((Layout<Tuple<>>::final_alignment), 0);
        static_assert(!is_trivial_and_standard_layout_v<Tuple<>>, "Tuple<> must not have standard layout.");

        auto builder = Builder<Tuple<>>();
        builder.finish();

        EXPECT_NE(builder.get_data(), nullptr);
        EXPECT_EQ(builder.get_size(), 1);  // minimum alignment requirement is 1
    }

    TEST(FlatmemoryTests, TypesTupleTest) {
        // Test without any padding.
        //  ______________________
        // ||  |  ||  |  ||  |  ||
        // ||  5  ||  6  ||  7  ||
        // ||__|__||__|__||__|__||
        EXPECT_EQ((Layout<Tuple<uint16_t, uint16_t, uint16_t>>::final_alignment), 2);
        static_assert(!is_trivial_and_standard_layout_v<Tuple<uint16_t, uint16_t, uint16_t>>, "Tuple<uint16_t, uint16_t, uint16_t> must not have standard layout.");

        auto builder = Builder<Tuple<int16_t, uint16_t, uint16_t>>();
        builder.get_builder<0>() = 5;
        builder.get_builder<1>() = 6;
        builder.get_builder<2>() = 7;
        builder.finish();
        EXPECT_NE(builder.get_data(), nullptr);
        EXPECT_EQ(builder.get_size(), 6);

        auto view = View<Tuple<int16_t, uint16_t, uint16_t>>(builder.get_data());
        EXPECT_EQ(view.get<0>(), 5);
        EXPECT_EQ(view.get<1>(), 6);
        EXPECT_EQ(view.get<2>(), 7);
    }


    TEST(FlatmemoryTests, TypesTuplePaddingTest) {
        // Test with padding in between.
        //  __________________________________________
        // ||  |  ||  |  ||  |  |  |  ||  |  ||  |  ||
        // ||  5  ||  P  ||     6     ||  7  ||  P  ||
        // ||__|__||__|__||__|__|__|__||__|__||__|__||
        EXPECT_EQ((Layout<Tuple<int16_t, int32_t, uint16_t>>::final_alignment), 4);
        static_assert(!is_trivial_and_standard_layout_v<Tuple<int16_t, int32_t, uint16_t>>, "Tuple<int16_t, int32_t, uint16_t> must not have standard layout.");

        auto builder = Builder<Tuple<int16_t, int32_t, uint16_t>>();
        builder.get_builder<0>() = 5;
        builder.get_builder<1>() = 6;
        builder.get_builder<2>() = 7;
        builder.finish();
        EXPECT_NE(builder.get_data(), nullptr);
        EXPECT_EQ(builder.get_size(), 12);

        auto view = View<Tuple<int16_t, int32_t, uint16_t>>(builder.get_data());
        EXPECT_EQ(view.get<0>(), 5);
        EXPECT_EQ(view.get<1>(), 6);
        EXPECT_EQ(view.get<2>(), 7);
    }


    TEST(FlatmemoryTests, TypesTupleVectorTest) {
        EXPECT_EQ((Layout<Tuple<Vector<uint64_t>>>::final_alignment), 8);
        static_assert(!is_trivial_and_standard_layout_v<Tuple<Vector<uint64_t>>>, "Tuple<Vector<uint64_t>> must not have standard layout.");

        auto builder = Builder<Tuple<Vector<uint64_t>>>();
        builder.get_builder<0>().get_builders().resize(3);
        builder.finish();
        EXPECT_NE(builder.get_data(), nullptr);
        EXPECT_EQ(builder.get_size(), 40);

        auto view = View<Tuple<Vector<uint64_t>>>(builder.get_data());
        EXPECT_EQ(view.get<0>().get_size(), 3);
    }


    TEST(FlatmemoryTests, TypesTupleVector2Test) {
        EXPECT_EQ((Layout<Tuple<Vector<uint64_t>, Vector<uint16_t>>>::final_alignment), 8);
        static_assert(!is_trivial_and_standard_layout_v<Tuple<Vector<uint64_t>, Vector<uint16_t>>>, "Tuple<Vector<uint64_t>, Vector<uint16_t>> must not have standard layout.");

        auto builder = Builder<Tuple<Vector<uint64_t>, Vector<uint16_t>>>();
        builder.get_builder<0>().get_builders().resize(3);
        builder.get_builder<1>().get_builders().resize(4);
        builder.finish();
        EXPECT_NE(builder.get_data(), nullptr);
        EXPECT_EQ(builder.get_size(), 56);

        auto view = View<Tuple<Vector<uint64_t>, Vector<uint16_t>>>(builder.get_data());
        EXPECT_EQ(view.get<0>().get_size(), 3);
        EXPECT_EQ(view.get<1>().get_size(), 4);
    }


    TEST(FlatmemoryTests, TypesTupleVectorVectorTest) {
        EXPECT_EQ((Layout<Tuple<Vector<Vector<uint8_t>>>>::final_alignment), 4);
        static_assert(!is_trivial_and_standard_layout_v<Tuple<Vector<Vector<uint8_t>>>>, "Tuple<Vector<Vector<uint8_t>>> must not have standard layout.");
 
        auto builder = Builder<Tuple<Vector<Vector<uint8_t>>>>();
        builder.get_builder<0>().get_builders().resize(3);
        builder.get_builder<0>().get_builders()[0].get_builders().resize(2);
        builder.get_builder<0>().get_builders()[1].get_builders().resize(3);
        builder.get_builder<0>().get_builders()[2].get_builders().resize(4);
        builder.finish();

        auto view = View<Tuple<Vector<Vector<uint8_t>>>>(builder.get_data());
        EXPECT_EQ(view.get<0>().get_size(), 3);
        EXPECT_EQ(view.get<0>()[0].get_size(), 2);
        EXPECT_EQ(view.get<0>()[1].get_size(), 3);
        EXPECT_EQ(view.get<0>()[2].get_size(), 4);
    }


    struct StructTest {
        uint32_t x;
        uint64_t y;
    };

    TEST(FlatmemoryTests, TypesTupleStructTest) {
        EXPECT_EQ((Layout<Tuple<StructTest>>::final_alignment), 8);
        static_assert(!is_trivial_and_standard_layout_v<Tuple<StructTest>>, "Tuple<StructTest> must not have standard layout.");
    
        auto builder = Builder<Tuple<StructTest>>();
        builder.get_builder<0>().x = 5;
        builder.get_builder<0>().y = 10;
        builder.finish();

        auto view = View<Tuple<StructTest>>(builder.get_data());
        EXPECT_EQ(view.get<0>().x, 5);
        EXPECT_EQ(view.get<0>().y, 10);
    }
}
