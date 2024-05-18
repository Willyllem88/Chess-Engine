#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/json.h>
#include <iostream>

class ChessApp : public cppcms::application {
public:
    ChessApp(cppcms::service &srv) : cppcms::application(srv) {}

    void main(std::string url) override {
        if (url == "/") {
            renderChessBoard();
        }
    }

private:
    void renderChessBoard() {
        response().out() <<
            "<html><head><title>Chess Board</title></head>"
            "<body><h1>Chess Board</h1>"
            "<table border='1'>";

        for (int row = 0; row < 8; ++row) {
            response().out() << "<tr>";
            for (int col = 0; col < 8; ++col) {
                if ((row + col) % 2 == 0) {
                    //Dark squares
                    response().out() << "<td style='background-color: #4e7b4a; width: 50px; height: 100px;'></td>";
                } else {
                    //Light squares
                    response().out() << "<td style='background-color: #f3f8d4; width: 100px; height: 50px;'></td>";
                }
            }
            response().out() << "</tr>";
        }

        response().out() << "</table></body></html>";
    }
};

int main(int argc, char **argv) {
    try {
        cppcms::service srv(argc, argv);
        srv.applications_pool().mount(cppcms::applications_factory<ChessApp>());
        srv.run();
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

/*
OPTIONS = -Wall -Wextra -Werror -Wno-sign-compare -std=c++20 -lcppcms -lbooster

engine.exe: board main.o board.o piece.o 
	g++ -o program.exe *.o

board: board.cpp board.hh
	g++ -c board.cpp $(OPTIONS)

board: board.cpp board.hh
	g++ -c board.cpp $(OPTIONS)

engine.o:
	g++ -c engine.cpp $(OPTIONS)

clean:
	rm -f *.o *.exe

*/