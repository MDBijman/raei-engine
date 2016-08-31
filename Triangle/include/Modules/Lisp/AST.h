#pragma once
#include <string>
#include <vector>

namespace Lisp
{
	class Node
	{
	public:
		Node(std::string content) : content(content) {}
		void addNode(Node n)
		{
			children.push_back(n);
		}

		std::string content;
		std::vector<Node> children;
	};

	class AST
	{
	public:
		AST(Node n) : root(n)
		{

		}

		Node root;
	};

	class ASTBuilder
	{
	public:
		ASTBuilder()
		{

		}

		AST build(const std::string& source)
		{
			checkBrackets(source);

			std::string stripped = squishWhitespace(removeOuterBrackets(source));

			Node root(stripped);

			std::string::iterator lastSpace = stripped.begin();
			for (auto it = stripped.begin(); it != stripped.end(); ++it)
			{
				if (isspace(*it))
					root.addNode(Node(stripped.substr(lastSpace - stripped.begin(), it - lastSpace)));
			}
			root.addNode(Node(stripped.substr(lastSpace - stripped.begin(), stripped.end() - lastSpace)));

			return AST(root);
		}

	private:
		/*
			Checks if each bracket is matched in the string.
		*/
		void checkBrackets(const std::string& source)
		{
			int depth = 0;
			for (uint32_t i = 0; i < source.size(); i++)
			{
				if (depth < 0)
				{
					std::string errorLocation = std::to_string(i);
					throw std::runtime_error("Invalid Lisp, bracket error at character: " + errorLocation);
				}

				if (source.at(i) == '(')
				{
					depth++;
				}
				else if (source.at(i) == ')')
				{
					depth--;
				}
			}

			if (depth != 0)
			{
				throw std::runtime_error("Invalid Lisp, unmatched brackets: " + std::to_string(depth));
			}

		}

		/*
			Removes the two outer matching brackets in the string.
		*/
		std::string removeOuterBrackets(const std::string& source)
		{
			std::string stripped(source);
			size_t openingLocation = stripped.find_first_of('(');
			size_t closingLocation = stripped.find_last_of(')');

			if (openingLocation == std::string::npos)
				throw std::runtime_error("Opening bracket was not found in string: " + stripped);
			if (closingLocation == std::string::npos)
				throw std::runtime_error("Closing bracket was not found in string: " + stripped);

			stripped.erase(closingLocation, 1);
			stripped.erase(openingLocation, 1);

			return stripped;
		}

		/*
			Squishes all adjacent white space into a single space.
		*/
		std::string squishWhitespace(std::string& source)
		{

		}
	};
}
