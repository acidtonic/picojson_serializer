#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "../picojson_serializer.h"
#include "test_helpers.h"

namespace {
    struct Point {
        double x, y, z;

        friend class picojson::convert::access;
        template<class Archive>
        void json(Archive & ar)
        {
            ar & picojson::convert::member("x", x);
            ar & picojson::convert::member("y", y);
            ar & picojson::convert::member("z", z);
        }
    };

    struct NamedPoint {
        std::string name;
        Point point;

        friend class picojson::convert::access;
        template<class Archive>
        void json(Archive & ar)
        {
            ar & picojson::convert::member("name", name);
            ar & picojson::convert::member("point", point);
        }
    };
}

TEST_CASE() {

    SECTION("serialization") {

        Point p = { 1, 2, 3 };
        picojson::value pv = picojson::convert::to_value(p);
        CHECK(has<double>(pv, "x", 1));
        CHECK(has<double>(pv, "y", 2));
        CHECK(has<double>(pv, "z", 3));
        
        std::string test_point_name("test point");
        NamedPoint np = { test_point_name , p };

        picojson::value npv = picojson::convert::to_value(np);
        CHECK(has<std::string>(npv, "name", test_point_name));
        CHECK(has<picojson::object>(npv, "point"));

        SECTION("deserialization from value") {
            Point np_ = { 0, 0, 0 };
            picojson::convert::from_value(pv, np_);
            CHECK(np_.x == 1);
            CHECK(np_.y == 2);
            CHECK(np_.z == 3);
            
            NamedPoint nnp = { "", { 0, 0, 0 } };
            picojson::convert::from_value(npv, nnp);
            CHECK(nnp.name == test_point_name);
            CHECK(nnp.point.x == 1);
            CHECK(nnp.point.y == 2);
            CHECK(nnp.point.z == 3);

            SECTION("deserialization from string") {
                std::string ps = picojson::convert::to_string(p);
                Point pss = { 0, 0, 0 };
                picojson::convert::from_string(ps, pss);
                CHECK(pss.x == 1);
                CHECK(pss.y == 2);
                CHECK(pss.z == 3);

                std::string nps = picojson::convert::to_string(np);
                NamedPoint npss = { "", { 0, 0, 0 } };
                picojson::convert::from_string(nps, npss);
                CHECK(npss.name == test_point_name);
                CHECK(npss.point.x == 1);
                CHECK(npss.point.y == 2);
                CHECK(npss.point.z == 3);
            }
        }
    }
}
