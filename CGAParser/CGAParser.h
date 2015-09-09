#pragma once

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/foreach.hpp>

#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace cga {
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

	class CGARule {
	public:
		std::string op_name;
		std::string output_name;

	public:
		virtual std::string to_string() = 0;
	};

	class CGAExtrudeRule : public CGARule {
	public:
		double height;

	public:
		CGAExtrudeRule(double height, const std::string& output_name);
		std::string to_string();
	};

	class CGAOffsetRule : public CGARule {
	public:
		double offsetDistance;

	public:
		CGAOffsetRule(double offsetDistance, const std::string& output_name);
		std::string to_string();
	};

	class CGASplitRule : public CGARule {
	public:
		std::string splitAxis;

	public:
		CGASplitRule(const std::string& splitAxis);
		std::string to_string();
	};

	typedef std::map<std::string, CGARule*> CGARules;
}

namespace cga {
    template <typename Iterator>
    struct cga_grammar : qi::grammar<Iterator, CGARules(), ascii::space_type> {
        cga_grammar() : cga_grammar::base_type(start) {
			start =	+(lhs
				>> "-->"
				>> statement
				>> ";")							
				;

			statement = (
				qi::lit("extrude") >> "("
				>> qi::double_				
				>> ")"
				>> output_name
				)								[ qi::_val = phoenix::new_<CGAExtrudeRule>(qi::_1, qi::_2) ]
				|
				(
				qi::lit("offset") >> "("
				>> qi::double_				
				>> ")"
				>> output_name
				)								[ qi::_val = phoenix::new_<CGAOffsetRule>(qi::_1, qi::_2) ]
				|
				(
				qi::lit("split") >> "("
				>> text
				>> ")"
				)								[ qi::_val = phoenix::new_<CGASplitRule>(qi::_1) ]
				;

			lhs = ascii::char_("a-zA-Z") >> *(ascii::char_("a-zA-Z"));
			text = ascii::char_("a-zA-Z") >> *(ascii::char_("a-zA-Z"));
			output_name = *(ascii::char_("a-zA-Z"));
        }

        qi::rule<Iterator, CGARules(), ascii::space_type> start;
        qi::rule<Iterator, CGARule*(), ascii::space_type> statement;
		qi::rule<Iterator, std::string(), ascii::space_type> lhs;
		qi::rule<Iterator, std::string(), ascii::space_type> text;
		qi::rule<Iterator, std::string(), ascii::space_type> output_name;
    };
}