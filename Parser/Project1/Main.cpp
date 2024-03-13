#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>



class parser_ini {
	std::ifstream in;
	std::string path;
	std::string section;
	std::string line;
	std::string param;
	std::vector<char> parametr;
	int* num_of_pole = new int(1);
	std::vector<std::pair<std::string, std::string>> data;
	bool true_section = false;
	std::set<std::string> list_of_param;
	bool adress_confirmed = 0;
	bool not_find = true;
	bool stop_bit = false;

	void catch_param(std::string par) {
		    list_of_param.insert(par);
	}

	bool decode(std::string word) {
		section.clear();
		param.clear();
		std::string tmp;
		bool f = 0;
		for (auto n : word) {
			if (n == '.') {
				section = tmp;
				tmp.clear();
				f = 1;
			}
			else {
				tmp.push_back(n);
			}

		}
		param = tmp;
		if (f == 1) {
			return 1;
		}
		else {
			return 0;
		}
	}

	std::string read() {
		int type = 0;
		bool write = 0;
		while (!in.eof()) {
			getline(in, line);
			type = check();
			if (type == 1) {
				write = 1;
				not_find = false;
			}
			else if (type == 3) {
				write = 0;
			}
			else if (type == 2 && write) {
				form_param(parametr);
			}
			++*num_of_pole;
		}
		if (true_section == false && not_find && !stop_bit) {
			throw
				std::out_of_range("Section not find in file!\n");
		}
		return find();
	}

	std::string find() {
		std::string str;
		bool found = 0;
		for (int i = 0; i < data.size(); ++i){
			if (data[i].first == param && !stop_bit) {
				str = data[i].first + ' ' + '=' + ' ' + data[i].second + "\n";
				found = 1;
			}
			else {
			}		
		}
		if (!found && !stop_bit) {
			for (auto i : data) {
					catch_param(i.first);
			}
			if (list_of_param.size() == 0) {
				str = "Parametr list not find in this section!\n";
			}
			else {
				str += "Заданного параметра не найдено! Может быть вы искали эти параметры?: \n";
				for (auto t : list_of_param) {
					str += t + '\n';
				}
			}
			
		}
		return str;
	}

	bool true_name(std::string name) {
		if (name == section) {
			true_section = true;
			return 1;
		}
		else {
			true_section = false;
			return 0;
		}
	}

	int check() {
		int v = 0;
		for (auto n : line) {
			if (n == ';' || stop_bit) {
				break;
			}
			else if (n == '[') {
				try {
					check_sec() ? v = 1 : v = 3;
				}
				catch (std::exception& er) {
					std::cerr << er.what() << path << "! Line: " << *num_of_pole << std::endl;
				}
				break;
			}
			else if (n != ';' && n != ' ' && n != '[' && !stop_bit) {
				try {
					check_param() ? v = 2 : v = 0;
				}
				catch (std::out_of_range& er) {
					std::cerr << er.what() << *num_of_pole << std::endl;
				}
				break;
			}

		}
		return v;
	}

	bool check_param() {
		std::string::iterator be;
		bool equal = 0;
		bool read = 0;
		bool stop = 0;
		parametr.clear();
		for (auto n = line.begin(); n < line.end(); ++n) {
			if (stop == 1) {
				break;
			}
			if (*n != ' ' && *n != ';' && read != 1) {
				be = n;
				for (auto i = be; i < line.end(); ++i) {
					if (*i != ';') {
						parametr.push_back(*i);
						read = 1;
					}
					else {
						stop = 1;
						break;
					}
					if (*i == '=') {
						equal = 1;
					}
				}
			}
		}
		if (read == 1 && equal == 1) {
			return 1;
		}
		else {
			stop_bit = true;
			throw
			std::out_of_range("Wrong parametr construction! Line: ");
			return 0;
		}
	}

	void form_param(std::vector<char> p) {
		std::string par;
		std::string v;
		std::string tmp;
		std::string::iterator it;
		for (auto n = p.begin(); n < p.end(); ++n) {
			if (*n != ' ' && *n != '=') {
				tmp.push_back(*n);
			}
			else if (*n == '=') {
				par = tmp;
				tmp.clear();
				++n;
				for (auto i = n; i < p.end(); ++i) {
					if (*i != ' ') {
						tmp.push_back(*i);
						++n;
					}
					else if (*n == ' ') {
						continue;
					}
					else if (*n == ';') {
						v = tmp;
						break;
					}
				}
				break;
			}
		}
		v = tmp;
		data.push_back({ par, v });
	}

	bool check_sec() {
		bool a = 0, b = 0;
		int a_ = 0, b_ = 0;
		std::string::iterator be;
		std::string::iterator en;
		std::string name;
		for (std::string::iterator i = line.begin(); i < line.end(); ++i) {
			
			if (*i == '[') {
				a = 1 - a_;
				a++;
				be = i;
			}if (*i == ']') {
				b = 1 - b;
				b++;
				en = i;
			}
		}
		if (a == 1 && b == 1) {
			for (auto i = be + 1; i < en; ++i) {
				if (*i == ' ') {
					stop_bit = true;
					throw
						std::exception("Wrong name of section in current file ");
					return false;
				}
				else {
					name.push_back(*i);
				}
			}
		}
		else {
			stop_bit = true;
			throw
				std::exception("Wrong name of section in current file ");
		}
		return true_name(name);
			
	}

	


public:
	parser_ini(std::string path_) {
		path = path_;
		in.open(path);
		if (!in.is_open()) {
			throw std::invalid_argument("Can't open this file! Try again to enter correct path!\n");
		}
		adress_confirmed = true;
	}
	parser_ini() {
	}
	void operator= (std::string s) {
		path = s;
		in.open(path);
		if (!in.is_open()) {
			throw std::invalid_argument ("Can't open this file! Try again to enter correct path!\n");
		}
		adress_confirmed = true;
	}
	~parser_ini() {
		in.close();
	}
	
	std::string get_value(std::string section_val) {
		bool right = decode(section_val);
		std::string str;
		if (right && !stop_bit) {
			try {
				str = read();
			}
			catch (std::out_of_range& er) {
				std::cerr << er.what() << std::endl;
			}
		}
		else {
			throw
				std::out_of_range("Wrong format of data\n");
		}
		return str;
	}
	bool get_adress() {
		return adress_confirmed;
	}
	void clear() {
		section.erase();
		param.erase();
		line.erase();
		parametr.clear();
		data.clear();
		list_of_param.erase(list_of_param.begin(), list_of_param.end());
		true_section = false;
		*num_of_pole = 1;
		in.seekg(0, std::ios_base::beg);
		not_find = true;
		stop_bit = false;
	}
};


int main() {
	setlocale(LC_ALL, "rus");
	parser_ini parser;
	std::string path;
	std::string value;
	std::string sec_val;
	while (!parser.get_adress()) {
		std::cout << "Введите имя файла: ";
		std::cin >> path;
		try {
			parser = path;
		}
		catch (std::invalid_argument& er)
		{
			std::cerr << er.what() << std::endl;
		}
	}
	while (sec_val !=  "0") {
		std::cout << "Введите название секции и искомый параметр в формате \"секция.параметр\"\nили введите \"0\" для выхода: ";
		std::cin >> sec_val;
		if (sec_val != "0") {
			try {
				value = parser.get_value(sec_val);
			}
			catch (std::out_of_range& er) {
				std::cerr << er.what() << std::endl;
			}
			std::cout << value;
		}
		parser.clear();
	}
	return 0;
}