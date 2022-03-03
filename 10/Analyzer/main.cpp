#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char* argv[]){
    if(argc == 0){
        std::cout << "Usage: ./JackAnalyzer file1.jack file2.jack...." << std::endl;

        return 1;
    }

    else{
        std::string fileName;

        for(int i = 1; i < argc; i++){
            fileName = argv[i];

            int fileExt = fileName.find(".jack");

            if(fileExt == std::string::npos){
                continue;
            }

            else{
                int dotPosition = fileName.find('.');
                std::string outputFileName = fileName.substr(0, dotPosition);
                outputFileName += "T.xml";
                std::ofstream outputFile;
                outputFile.open(outputFileName);

                if(!outputFile.is_open()){
                    std::cerr << "Error creating output file." << std::endl;
                    return 1;
                }

                outputFile << "<tokens>" << std::endl;

                JackTokenizer jt(fileName);
                
                while(jt.hasMoreTokens()){
                    jt.advance();
                }

                for(int a = 0; a < jt.tokens.size(); a++){
                    if(jt.tokens[a].length() > 0){
                        if(jt.tokenType(jt.tokens[a]) == "KEYWORD"){
                            outputFile << "<keyword> " << jt.tokens[a] << " </keyword>" << std::endl;
                        }

                        else if(jt.tokenType(jt.tokens[a]) == "SYMBOL"){
                            if(jt.tokens[a][0] == '<'){
                                outputFile << "<symbol> &lt; </symbol>" << std::endl;
                            }

                            else if(jt.tokens[a][0] == '>'){
                                outputFile << "<symbol> &gt; </symbol>" << std::endl;
                            }

                            else if(jt.tokens[a][0] == '"'){
                                outputFile << "<symbol> &quot; </symbol>" << std::endl;
                            }

                            else if(jt.tokens[a][0] == '&'){
                                outputFile << "<symbol> &amp; </symbol>" << std::endl;
                            }

                            else{
                                outputFile << "<symbol> " << jt.tokens[a] << " </symbol>" << std::endl;
                            }
                        }

                        else if(jt.tokenType(jt.tokens[a]) == "INT_CONST"){
                            outputFile << "<integerConstant> " << jt.tokens[a] << "</integerConstant>" << std::endl;
                        }

                        else if(jt.tokenType(jt.tokens[a]) == "STRING_CONST"){
                            outputFile << "<stringConstant> " << jt.tokens[a].substr(1, jt.tokens[a].length() - 2) 
                                << " </stringConstant>" << std::endl;
                        }

                        else{
                            outputFile << "<identifier> " << jt.tokens[a] << " </identifier>" << std::endl;
                        }
                    }
                }

                outputFile << "</tokens>" << std::endl;

                CompilationEngine ce(fileName, &jt);
            }
        }

        return 0;
    }
}