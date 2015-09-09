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

namespace client {
    namespace fusion = boost::fusion;
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
		CGAExtrudeRule(double height, std::string& output_name) : height(height) { this->output_name = output_name; }
		std::string to_string() {
			std::ostringstream oss;
			oss << "extrude(" << height << ") " << output_name;
			return oss.str();
		}
	};

	typedef std::map<std::string, CGARule*> CGARules;
}

namespace client {
	namespace phoenix = boost::phoenix;

    // 自前で実装するgrammarは、qi::grammarを継承させる。
	// qi::grammarの2番目の引数には、結果を格納するオブジェクトを指定する。
	// base_type()の引数が、axiomに相当する。
    template <typename Iterator>
    struct cga_grammar : qi::grammar<Iterator, CGARules(), ascii::space_type> {
        cga_grammar() : cga_grammar::base_type(start) {
            using qi::lit;
            using qi::lexeme;
			using qi::double_;
            using ascii::char_;
            using ascii::string;
            using namespace qi::labels;
			using boost::spirit::attr;

            using phoenix::at_c;
            using phoenix::push_back;

			start =	+(text
				>> "-->"
				>> statement
				>> ";")							
				;

			statement = (
				qi::lit("extrude") >> "("
				>> qi::double_				
				>> ")"
				>> output_name
				)								[ qi::_val = boost::phoenix::new_<CGAExtrudeRule>(qi::_1, qi::_2) ]
				;

			text = char_("a-zA-Z") >> *(char_("a-zA-Z"));
			output_name = *(char_("a-zA-Z"));

        }

        qi::rule<Iterator, CGARules(), ascii::space_type> start;
        qi::rule<Iterator, CGARule*(), ascii::space_type> statement;
		qi::rule<Iterator, std::string(), ascii::space_type> text;
		qi::rule<Iterator, std::string(), ascii::space_type> output_name;
    };
}

int main(int argc, char **argv) {
    char const* filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in) {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }

    std::string storage; // We will read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping!
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(storage));

    typedef client::cga_grammar<std::string::const_iterator> cga_grammar;
    cga_grammar g; // Our grammar
    client::CGARules cga_rules; // Our tree

    using boost::spirit::ascii::space;
    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();

	/// EmployeeParserと同様、3番目の引数がgrammarだ。つまり、mini_xml_grammarを使ってparseする。
	// 4番目の引数はskipする文字。つまり、空白をskipする
	// 5番目の引数に結果が返却される。
    bool r = phrase_parse(iter, end, g, space, cga_rules);

    if (r && iter == end) {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "-------------------------\n";
		for (auto it = cga_rules.begin(); it != cga_rules.end(); ++it) {
			std::cout << it->first << " --> " << it->second->to_string() << std::endl;
		}
        return 0;
    } else {
        std::string::const_iterator some = iter+30;
        std::string context(iter, (some>end)?end:some);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "stopped at: \": " << context << "...\"\n";
        std::cout << "-------------------------\n";
        return 1;
    }
}