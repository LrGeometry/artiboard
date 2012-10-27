#include "command.h"
#include <sstream>
#include <vector>
#include "../systemex/systemex.h"
#include <fstream>
#include <assert.h>
#include <time.h>
#include "../systemex/templates.h"

namespace interpreter
{

	using std::ifstream;
	using systemex::runtime_error_ex;
	using systemex::parse_error;

	Context::Context()
	{
		_timer_is_on = false;
	}

	const string& Context::variable(const string& n) const
	{
		std::map<string,string>::const_iterator f = _vars.find(n);
		if (f == _vars.end())
			throw runtime_error_ex(
			"value for variable name '%s' expected during execution of command '%s'",
			n.c_str(), _command.c_str());
		return f->second;
	}

	const string& Context::argument(const unsigned int n) const
	{
		if (n >= _args.size())
			throw runtime_error_ex(
			"the command '%s' expects an argument at position %d",_command.c_str(),n+1);
		return _args[n];
	}

	const string& Context::argument(const unsigned int n, const string& def_value) const
	{
		if (n >= _args.size())
			return def_value;
		return _args[n];
	}

	void Context::append_arguments(const unsigned int from, string &target) const
	{
		if (from < _args.size())
		{
			if (target.size() != 0)
				target += ' ';
			target += _args[from];
			append_arguments(from+1,target);
		}
	}

	int Context::argument_as_int(const unsigned int n) const
	{
		std::istringstream num_stream(argument(n));
		int result;
		num_stream >> result;
		return result;
	}

	void Context::get_value(const string& literal, string& target) const
	{
		if (literal[0] == '$')
		{
			string variable_name;
			variable_name.assign(literal.c_str()+1);
			if (variable_name[0] == '$')
			{
				get_value(variable_name,target);
				variable_name = target;
			}
			target = operator [](variable_name); 
		}
		else
			target = literal;
	}

	void Context::set_variable(const string& name, const string& value)
	{
		string v;
		get_value(value,v);
		_vars[name] = v; 
	}

	void Context::run_command(const string& name, const std::vector<string>& args)
	{
		try
		{
			_command = name;
			word_list::const_iterator it;
			string v;
			_args.clear();
			for (it = args.begin(); it != args.end(); it++)
			{	
				get_value(*it,v);
				_args.push_back(v);
			}
			if (name == "?")  // print
			{
				for (it = _args.begin(); it != _args.end(); it++)
					std::cout << *it << " ";
				std::cout << std::endl;
			}
			else if (name == "timer")
			{
				if (args.size() != 0)
				{
					if (args[0] == "on")
						_timer_is_on = true;
					else if (args[0] == "off")
						_timer_is_on = false;
					else
						throw parse_error("timer [on|off]", args[0].c_str());
				}
				std::cout << "timer is " <<  (_timer_is_on?"on":"off") << std::endl; 
			}
			else if (name == "!")
			{
				string cmdArgs;
				append_arguments(0,cmdArgs);
				std::cout << "executing " << cmdArgs << std::endl;
				system(cmdArgs.c_str());
			}
			else if (name == "runfile")
			{
				string filename;
				append_arguments(0,filename);
				ifstream command_stream;
				command_stream.open(filename.c_str());
				if (!command_stream)
					throw runtime_error_ex("could not open file '%s'",filename.c_str());
				interpreter::Block b;
				b.parse(command_stream);
				std::cout << "parsed " <<  filename << std::endl;
				b.interpret(*this);
			}
			else
			{
				clock_t start,end;
				start = clock();
				std::cout << "executing " << name << " " <<  _args << std::endl;
				execute();
				if (_timer_is_on)
				{
					end = clock();
					std::cout << name << " elapsed " 
						<< float (end - start) / CLOCKS_PER_SEC << std::endl;
				}
			}
		}
		catch (...)
		{
			throw;
		}
	}

	static char buffer[1024];
	Statement * Statement::parse_line(std::istream &statement_stream)
	{
		
		statement_stream.getline(buffer,1024);
		string statement = buffer;

		// replace tabs with spaces
		for (string::iterator it = statement.begin();
			it != statement.end(); it++)
			if (*it == '\t')
				*it = ' ';

		std::istringstream stream(statement + " ");
		// break up in words
		std::vector<string> words;
		while (stream)
		{
			stream.getline(buffer,1024,' ');
			if (buffer[0] != 0)
				words.push_back(buffer);
		}
		if (words.size() == 0 || words[0][0] == '#')
			return 0;  // empty statement
		if (words.size() > 2 && words[1] == "=") // set statement
		{
			word_list expression;	
			for (size_t i = 1; i < words.size(); i++)
				expression.push_back(words[i]);
			return new SetOperation(words[0], expression);
		}
		else 
			return new Command(words);
	}

	void SetOperation::interpret(Context& c)
	{
		string value = "";				
		word_list::iterator it;
		string v;
		for (it = _args.begin(); it != _args.end(); it++)
		{
			c.get_value(*it,v);
			value += v;
		}
		c.set_variable(_name, value );
		std::cout << _name << " is " << c[_name] << std::endl;
	}

	Command::Command(const std::vector<string> & args)
	{
		std::vector<string>::const_iterator it = args.begin();
		assert(it != args.end());
		_name = *it;
		it++;
		_args.assign(it,args.end());
	}

	void Command::interpret(Context& c)
	{
		c.run_command(_name,_args);
	}

	void Block::parse(std::istream &statement_stream)
	{
		_statements.clear();
		while (statement_stream)
		{
			Statement * s = Statement::parse_line(statement_stream);
			if (s != 0)
				_statements.push_back(s);	
		}
	}

	void Block::interpret(Context& c)
	{
		collection::iterator it;
		for (it = _statements.begin(); it != _statements.end(); it++)
			(*it)->interpret(c);
	}

	Block::~Block()
	{
		collection::iterator it;
		for (it = _statements.begin(); it != _statements.end(); it++)
			delete *it;
	}


}

