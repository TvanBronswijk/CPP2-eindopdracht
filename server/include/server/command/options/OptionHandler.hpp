#pragma once
#include <vector>
#include <functional>

namespace server::command::options {
	class OptionHandler {
	public:
		OptionHandler(std::vector<std::string> options, std::function<bool(int)> function)
		: _options(std::move(options)), _func(std::move(function)) {}
		bool choose(int choice) {
			if(choice >= 0 && choice < _options.size())
				return _func(choice);
			else
				throw;
		}

		template<class Stream> friend Stream &operator<<(Stream &str, const OptionHandler &opt);
	private:
		std::function<bool(int)> _func;
		std::vector<std::string> _options;
	};

	template<class Stream> inline Stream &operator<<(Stream &str, const OptionHandler &opt) {
		for(int i = 0; i < opt._options.size(); i++)
			str << '[' << std::to_string(i) << "] " << opt._options[i] << "\r\n";
		return str;
	}
}

