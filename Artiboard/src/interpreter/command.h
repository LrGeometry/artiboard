#pragma once
#include <string>
#include <map>
#include <vector>
/*
Interpreter implements the following syntax:
	statement ::= set_operation | command
	set_operation ::= variable_name ' = ' value
	query_operations ::= '? ' variable_name
	command ::= 'do' command_name
	value ::=  string 
	command_name ::= string

	A line of comment starts with #
 */

namespace interpreter
{
	using std::string;
	typedef std::vector<string> word_list;

	class Context
	{
	private:
		std::map<string,string> _vars;
		word_list _args;
		string _command;
		bool _timer_is_on;
	protected:
		const string& command() const
		{ return _command; }
		virtual void execute() =  0; // run the current command
	public:
		Context();
		void append_arguments(const unsigned int from, string &target) const;
		const string& variable(const string& name) const;
		const string& argument(const unsigned int index) const;
		const string& argument(const unsigned int index, const string& def) const;
		int argument_as_int(const unsigned int index) const;
		const string& operator[] (const string& name) const	{ return variable(name); }
		void set_variable(const string& name, const string& value);
		void get_value(const string& literal, string& target) const;
		void run_command(const string& name,const word_list& args);
		virtual ~Context() {}
	};

	class Statement
	{
	public:
		static Statement * parse_line(std::istream &stream);
		virtual void interpret(Context& c) = 0;
		virtual ~Statement(){};
	};

	 
	class Block : Statement  // a block of statements
	{
	private:
		typedef std::vector<Statement *> collection;
		collection _statements;
	public:
		void parse(std::istream &stream);
		virtual void interpret(Context& c);
		virtual ~Block();
	};

	class Command : public Statement
	{
	protected:
		string _name;
		word_list _args;
	public:
		Command(const word_list& args);
		virtual void interpret(Context& c);
	};

	class SetOperation : public Command
	{
	private:
		string _name; 
	public:
		SetOperation(const std::string& name,
			const word_list& args):Command(args){_name = name;}
		virtual void interpret(Context& c);
	};

}


