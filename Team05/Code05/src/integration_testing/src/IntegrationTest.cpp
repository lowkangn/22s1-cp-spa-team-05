#include "catch.hpp"
#include <string>
#include <unordered_set>
#include <sstream>

#include <pkb/PKB.h>
#include <qps/QPS.h>
#include <sp/SourceProcessor.h>

using namespace std;

TEST_CASE("Integration") {

	// inputs
	string program = "procedure z { \n   x = x;\n   x = x + 1000 - z + i;\n   read x;\n   while ( 1 < 2 ) {\n  \tif ( ( (k) < 10 ) && ( j * 2 < 5 )) then {\n    \t\ty = x + 20 / z + 2 * 0 / ( 5 + ((0)) ) + z + 1 / 2;\n    \t\tread z;\n        } else {\n    \t\tz = z + 1 / 2;\n\t\twhile ((0 != (i + 1) % ((2))) || (!( ( (((z))) + 1 ) < 2 )) ) {\n\t\t\tprint y;\n\t\t\tif ( (1 == 1) && ((0)+2 < 5 ) ) then {\n\t\t\t\tread y;\n\t\t\t} else {\n\t\t\t\tread z;\n\t\t\t}\n\t\t}\n        }\n  \tz = x + z - 9;\n  \tcall z1;\n    }\n}\n\nprocedure z1 { \n   x = x;\n   x = x + 1000 - z + i;\n   read x;\n   while ( 1 < 2 ) {\n  \tif ( ( (k) < 10 ) && ( j * 2 < 5 )) then {\n    \t\ty = x + 20 / z + 2 * 0 / ( 5 + ((0)) ) + z + 1 / 2;\n    \t\tread z;\n        } else {\n    \t\tz = z + 1 / 2;\n\t\twhile ((0 != (i + 1) % ((2))) || (!( ( (((z))) + 1 ) < 2 )) ) {\n\t\t\tprint y;\n\t\t\tif ( (1 == 1) && ((0)+2 < 5 ) ) then {\n\t\t\t\tread y;\n\t\t\t} else {\n\t\t\t\tread z;\n\t\t\t}\n\t\t}\n        }\n  \tz = x + z - 9;\n  \tcall z2;\n    }\n}\n\n\nprocedure z2 { \n   x = x;\n   x = x + 1000 - z + i;\n   read x;\n   while ( 1 < 2 ) {\n  \tif ( ( (k) < 10 ) && ( j * 2 < 5 )) then {\n    \t\ty = x + 20 / z + 2 * 0 / ( 5 + ((0)) ) + z + 1 / 2;\n    \t\tread z;\n        } else {\n    \t\tz = z + 1 / 2;\n\t\twhile ((0 != (i + 1) % ((2))) || (!( ( (((z))) + 1 ) < 2 )) ) {\n\t\t\tprint y;\n\t\t\tif ( (1 == 1) && ((0)+2 < 5 ) ) then {\n\t\t\t\tread y;\n\t\t\t} else {\n\t\t\t\tread z;\n\t\t\t}\n\t\t}\n        }\n  \tz = x + z - 9;\n  \tread z2;\n    }\n}";
	unordered_set<string> output = {};
	string query = "while w, w1; if i, i1; read r; variable v, v1; assign a, a1; stmt s; constant const; Select <const> such that Uses(w, v) with 12345 = 12345 such that Modifies(i, v1)  pattern i1(_,_,_) with r.varName=\"z\" with s.stmt#=46 such that Calls*(\"z\",\"z2\") pattern a(\"x\", \"x\") pattern a1 (_, _\"1000\"_)";

	// given
	shared_ptr<PKB> pkbPointer = shared_ptr<PKB>(new PKB());
	QPS qps = QPS();
	stringstream ss(program);
	SourceProcessor sp = SourceProcessor(ss);

	// when
	//extract
	sp.extractAllAndAddToPkb(pkbPointer);

	// qps parse query
	qps.evaluate(query, shared_ptr<PKBQueryHandler>(pkbPointer));

	// project
	list<string> results;
	qps.projectResults(results);

	// then 
	for (string r : results) {
		REQUIRE(output.find(r) != output.end());
	}

}