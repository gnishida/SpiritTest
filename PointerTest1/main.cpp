#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

class Expr
{
    public:
        virtual ~Expr() = 0;
};
Expr::~Expr() {}

class Plus : public Expr
{
    public:
        Plus (Expr* e1, Expr* e2) : _e1(e1), _e2(e2) {}
        ~Plus () {}
    private:
        Expr* _e1;
        Expr* _e2;
};

class Num : public Expr
{
    public:
        Num(double n) : _n(n) {}
    private:
        double _n;
};

template <typename Iterator>
struct ExprParser : qi::grammar<Iterator, Expr*(), ascii::space_type>
{

    ExprParser()
      : ExprParser::base_type (start, "expr")
    {
        expr = qi::double_     [ qi::_val = phoenix::new_<Num>(qi::_1) ]
                | ('('
                    >> expr
                    >> '+'
                    >> expr
                  >> ')')      [ qi::_val = phoenix::new_<Plus>(qi::_1, qi::_2) ]
                ;

        start %= expr;
    }

    qi::rule<Iterator, Expr*(), ascii::space_type> expr;
    qi::rule<Iterator, Expr*(), ascii::space_type> start;
};

int main(int argc, const char *argv[])
{
    char input[] = "(3.14 + 39)";
    ExprParser<const char*> p;

    const char *f = std::begin(input);
    const char *l = std::end(input);
	Expr* result;
    if (qi::phrase_parse(f, l, p, ascii::space, result)) {
		// ここでbreakして、resultを見ると、
		// Plusオブジェクトであることが分かる。
		std::cout << "OK" << std::endl;
	}
}