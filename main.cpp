#include <iostream>
#include <unordered_map>

#include "Unordered_Map.h"
#include "Dictionary.h"
#include "Timer.h"

int main() {
	/*Dictionary<int, int> dic;
	Unordered_Map<int, int> map;
	std::unordered_map<int, int> stdMap;

	srand(time(NULL));

	int n;
	std::cin >> n;
	std::vector<int> random;
	for (size_t i = 0; i < n; i++)
		random.push_back(rand() % 500);


	std::cout << "Dictionary\n";
	{
		Timer timer;

		for (size_t i = 0; i < n; i++)
			dic.insert({random[i], i });
	}
	std::cout << "\nUnordered Map\n";
	{
		Timer timer;

		for (size_t i = 0; i < n; i++)
			map.insert({ random[i], i });
	}
	std::cout << "\nStandard Unordered Map\n";
	{
		Timer timer;
		
		for (size_t i = 0; i < n; i++)
			stdMap.insert({ random[i], i });
	}

	std::cout << "\n\nErase\n\n";


	std::cout << "Dictionary\n";
	{
		Timer timer;

		int count = 0;
		for (const auto& el : dic) {
			auto miau = dic.get(el.first);
			std::cout << miau;
			count++;
		}
		std::cout << std::endl << count;
	}
	std::cout << "\nUnordered Map\n";
	{
		Timer timer;

		int count = 0;
		for (const auto& el : map) {
			auto miau = map.find(el.first);
			std::cout << miau->second;
			count++;
		}
		std::cout << std::endl << count;
	}
	std::cout << "\nStandard Unordered Map\n";
	{
		Timer timer;

		int count = 0;
		for (const auto& el : stdMap) {
			auto miau = stdMap.find(el.first);
			std::cout << miau->second;
			count++;
		}
		std::cout << std::endl << count;
	}*/


	std::unordered_map<int, int> map;
	map.emplace(1, 69);
	map.emplace(1, 70);

	std::cout << map[1];

	std::cin.get();
}


