#include "CGAParser.h"

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

    cga::cga_grammar<std::string::const_iterator> g;
    cga::CGARules cga_rules; // Our tree

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