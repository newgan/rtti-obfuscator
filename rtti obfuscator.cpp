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
	std::cout << "enter an input path: ";

	std::srand(time(0));

	try {
		// path to input binary
		std::string path;
		std::cin >> path;

		std::ifstream fs(path, std::fstream::binary);
		if (fs.fail()) {
			throw std::exception("Could not open source binary");
		}

		std::cout << "enter an output path: (type 'b' to replace input path): ";
		std::string output_path;
		std::cin >> output_path;

		// read file contents
		std::stringstream ss;
		ss << fs.rdbuf();
		auto contents = ss.str();

		std::regex reg(R"(\.(\?AV|PEAV|\?AU)(.+?)@@\0)");
		std::match_results<std::string::iterator> match;

		auto itr = contents.begin();

		const std::string replacement_characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
		const auto length = replacement_characters.length();

		while (std::regex_search(itr, contents.end(), match, reg)) {
			//for all characters of the regex match, pick a random character to replace it with
			do {
				for (auto letter = match[0].first; letter != match[0].second; ++letter) {
					*letter = replacement_characters[std::rand() % length];
				}
			} while (replaced_rtti.find(match[0]) != replaced_rtti.end()); // try again in case there's a collision

			replaced_rtti.emplace(match[0]);

			itr += match.position() + match.length();
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