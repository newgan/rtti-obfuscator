#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <regex>
#include <filesystem>
#include <numeric>

std::unordered_set<std::string> replaced_rtti{};

int main(int argc, char* argv[]) {
	std::cout << "===== rtti obfuscator =====" << std::endl;
	std::cout << "enter a file path: ";

	std::srand(time(0));

	try {
		// path to input binary
		std::string path;
		std::cin >> path;

		std::ifstream fs(path, std::fstream::binary);
		if (fs.fail()) {
			throw std::exception("Could not open source binary");
		}

		std::cout << "enter an output path: (type 'b' to replace input path)" << std::endl;
		std::string output_path;
		std::cin >> output_path;

		// read file contents
		std::stringstream ss;
		ss << fs.rdbuf();
		auto contents = ss.str();

		std::regex reg(R"(\.(\?AV|PEAV|\?AU)(.+?)@@\0)");
		std::match_results<std::string::iterator> res;

		auto itr = contents.begin();

		std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

		while (std::regex_search(itr, contents.end(), res, reg)) {
			do {
				for (auto j = res[0].first; j != res[0].second; ++j) {
					*j = letters[std::rand() % 62];
				}
			} while (replaced_rtti.find(res[0]) != replaced_rtti.end());

			replaced_rtti.emplace(res[0]);

			itr += res.position() + res.length();
		}

		// generate output path
		std::filesystem::path p(path);
		auto final_path = !output_path.compare("b") ? p.parent_path().string() + "\\" + p.stem().string() + p.extension().string() : output_path;

		// write to file
		std::ofstream os(final_path, std::ofstream::trunc | std::ofstream::binary);

		if (!os.write(contents.data(), contents.size())) {
			throw std::exception((std::string("unable to write to file ") + final_path).c_str());
		}
		else {
			std::cout << std::format("successfully obfuscated rtti information({} matches replaced). output written to ", std::to_string(replaced_rtti.size())) << final_path << std::endl;
			system("pause");
		}
	}

	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		system("pause");
		return 1;
	}

	return 0;
}