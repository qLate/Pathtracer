#include "Model.h"

#include <fstream>
#include <sstream>

Model::Model(const std::filesystem::path& path)
{
	parse(path);
}

constexpr char DELIM = '/';
void Model::parse(const std::filesystem::path& path)
{
	std::ifstream file(path);
	std::string line;

	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec2> vertexUVs;
	std::vector<glm::vec3> vertexNormals;
	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string token;
		ss >> token;
		if (token == "v")
		{
			std::vector<float> pos;
			while (ss >> token)
				pos.push_back(std::stof(token));

			vertexPositions.emplace_back(pos[0], pos[1], pos[2]);
		}
		else if (token == "vt")
		{
			std::vector<float> uv;
			while (ss >> token)
				uv.push_back(std::stof(token));

			vertexUVs.emplace_back(uv[0], uv[1]);
		}
		else if (token == "vn")
		{
			std::vector<float> normal;
			while (ss >> token)
				normal.push_back(std::stof(token));

			vertexNormals.push_back(normalize(glm::vec3(normal[0], normal[1], normal[2])));
		}
		else if (token == "f")
		{
			std::vector<int> posIndexes;
			std::vector<int> uvIndexes;
			std::vector<int> normalIndexes;

			int num1 = 0, num2 = 0, num3 = 0;
			std::string str, substr;
			while (ss >> str)
			{
				int ind1 = str.find(DELIM);
				substr = str.substr(0, ind1);
				num1 = std::stoi(substr);

				if (ind1 != std::string::npos)
				{
					int ind2 = str.find(DELIM, ind1 + 1);
					substr = str.substr(ind1 + 1, ind2);
					num2 = std::stoi(substr);

					if (ind2 != std::string::npos)
					{
						substr = str.substr(ind2 + 1);
						num3 = std::stoi(substr);
					}
				}

				posIndexes.push_back(num1 - 1);
				uvIndexes.push_back(num2 - 1);
				normalIndexes.push_back(num3 - 1);
			}

			for (int i = 2; i < posIndexes.size(); i++)
			{
				auto v1 = Vertex(vertexPositions[posIndexes[0] % vertexPositions.size()], uvIndexes[0] != -1 ? vertexUVs[uvIndexes[0]] : glm::vec2(),
				                 normalIndexes[0] != -1 ? vertexNormals[normalIndexes[0]] : glm::vec3(0, 0, 0));
				auto v2 = Vertex(vertexPositions[posIndexes[i - 1] % vertexPositions.size()], uvIndexes[i - 1] != -1 ? vertexUVs[uvIndexes[i - 1]] : glm::vec3(),
				                 normalIndexes[i - 1] != -1 ? vertexNormals[normalIndexes[i - 1]] : glm::vec3(0, 0, 0));
				auto v3 = Vertex(vertexPositions[posIndexes[i] % vertexPositions.size()], uvIndexes[i] != -1 ? vertexUVs[uvIndexes[i]] : glm::vec2(),
				                 normalIndexes[i - 1] != -1 ? vertexNormals[normalIndexes[i]] : glm::vec3(0, 0, 0));

				triangles.emplace_back(new Triangle(nullptr, v1, v2, v3));
			}
		}
	}
}
