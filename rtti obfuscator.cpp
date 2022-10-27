#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <time.h>
#include <cstdlib>
#include <unordered_set>
#include <regex>
#include <filesystem>

std::unordered_set<std::string> used_type_names;

std::string get_rand_str(const int len) {
	std::string s;
	s.resize(len);

	for (auto i = 0; i < len; ++i) {
		// just pick a random byte
		s[i] = rand() % 0xff;
	}

	s[len] = 0;

	return s;
}

std::unordered_set<std::string> replaced_rtti{};

int main(int argc, char* argv[]) {
	std::cout << "===== rtti obfuscator =====" << std::endl;

	srand(time(nullptr));

	try {
		// path to input binary
		std::string path;
		std::cin >> path;

		std::ifstream fs(path, std::fstream::binary);
		if (fs.fail()) {
			throw std::exception("Could not open source binary");
		}

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
					*j = letters[rand() % 62];
				}
			} while (replaced_rtti.find(res[0]) != replaced_rtti.end());

			replaced_rtti.emplace(res[0]);

			itr += res.position() + res.length();
		}

		// generate output path
		std::filesystem::path p(path);
		auto output_path = p.parent_path().string() + "\\" + p.stem().string() + p.extension().string();

		// write to file
		std::ofstream os(output_path, std::ofstream::trunc | std::ofstream::binary);
		if (!os.write(contents.data(), contents.size())) {
			throw std::exception((std::string("unable to write to file ") + output_path).c_str());
		}
		else {
			auto count = std::to_string(replaced_rtti.size() > 0 ? replaced_rtti.size() - 1 : 0);

			std::cout << std::format("successfully obfuscated rtti information({} matches replaced). output written to ", count) << output_path << std::endl;
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