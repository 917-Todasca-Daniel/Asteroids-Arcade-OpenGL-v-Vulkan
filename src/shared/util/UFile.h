#pragma once


#include <string>
#include <fstream>


namespace Assimp {
    class Importer;
}


namespace aa
{

    class UFile {

    public:
        static std::string readFileContent(const std::string& filename);

        static std::string normalizedDirectory(const std::string& directory);

        static Assimp::Importer* getAssimpFileReader();

        static void setAssimpFileReader(Assimp::Importer*);


    private:
        static const std::string ROOT;

        static Assimp::Importer* IMPORTER;

    };

}
