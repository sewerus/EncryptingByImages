#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<bitset>
#include "CImg-2.4.0/CImg.h"
using namespace cimg_library;

std::vector<std::string> lines;

std::string get_name_of_file(std::string title)
{
    std::cout << title;
    std::string file_name;
    std::cin >> file_name;
    return file_name;
}

std::string load_text(std::string text_file)
{
    lines.clear();
    //file_name
    lines.push_back(text_file);
    std::fstream input;
    std::string line;
    input.open(text_file.c_str(), std::ios::binary | std::ios::in);
    while(input.good())
    {
        std::getline(input, line, '\0');
        lines.push_back(line);
    }
    input.close();
    return "Wczytano tekst";
}

int length_of_longest_line()
{
    int length = 0;
    for(int i = 0; i < lines.size(); i++)
    {
        if(length < lines[i].length())
            length = lines[i].length();
    }
    return length;
}

char r_part_code(char x)
{
    //1,2,3 bits
    return x >> 5;
}

char g_part_code(char x)
{
    //4,5,6 bits
    x &= (char)31;
    return x >> 2;
}

char b_part_code(char x)
{
    //7,8 bits
    return x & (char)3;
}

char recreate_char(int r, int g, int b)
{
    return (r << 5) + (g << 2) + b;
}

void add_shadows(CImg <unsigned char> & img)
{
    char r,g,b;
    int index = 0;
    for(int i = 0; i < lines.size(); i++ )
    {
        for(int j = 0; j < lines[i].length(); j++)
        {
            r = r_part_code(lines[i][j]);
            g = g_part_code(lines[i][j]);
            b = b_part_code(lines[i][j]);
            img(index % img.width(), index / img.width(), 0, 0) += r;
            img(index % img.width(), index / img.width(), 0, 1) += g;
            img(index % img.width(), index / img.width(), 0, 2) += b;
            index++;
        }
        //break line
        index++;
    }
}

void show_shadows()
{
    load_text(get_name_of_file("Prosze podac nazwe pliku tekstowego, w ktorym jest tekst do zaszyfrowania:"));
    CImg<unsigned char> img(length_of_longest_line(),lines.size(),1,3,0);
    add_shadows(img);
    img.display("Shadows code");
    std::cout << std::endl;
}

std::string generate_coded_image(std::string key_image_file, std::string coded_image_file)
{
    CImg<unsigned char> img;
    img.load_bmp(key_image_file.c_str());
    //check if image size is enough
    if(sizeof(lines) > img.width() * img.height())
    {
        std::cout << "Try" << std::endl;
        return "Zbyt maly obraz, za duzo tekstu do zaszyfrowania!";
    }

    add_shadows(img);
    img.display(coded_image_file.c_str());
    img.save_bmp(coded_image_file.c_str());
    return "Wygenerowano obraz";
}

std::string decode_image(std::string coded_image_file, std::string key_image_file)
{
    CImg<unsigned char> coded_img;
    coded_img.load_bmp(coded_image_file.c_str());
    CImg<unsigned char> key_img;
    key_img.load_bmp(key_image_file.c_str());

    //check if width and height are the same
    if(coded_img.width() != key_img.width() || coded_img.height() != key_img.height())
    {
        return "Podane obrazy maja rozne wymiary!";
    }

    char r,g,b,character,different;
    //add empty line to lines
    lines.clear();
    lines.push_back("");
    //go pixel by pixel
    for(int index = 0; index < coded_img.width() * coded_img.height(); index++)
    {
            //get values from coded image
            r = coded_img(index % coded_img.width(), index / coded_img.width(), 0, 0);
            g = coded_img(index % coded_img.width(), index / coded_img.width(), 0, 1);
            b = coded_img(index % coded_img.width(), index / coded_img.width(), 0, 2);
            //compare with key images
            r -= key_img(index % key_img.width(), index / key_img.width(), 0, 0);
            g -= key_img(index % key_img.width(), index / key_img.width(), 0, 1);
            b -= key_img(index % key_img.width(), index / key_img.width(), 0, 2);

            //add new line or new character
            if(r + g + b == 0)
                lines.push_back("");
            else
                lines[lines.size() - 1] += recreate_char(r, g, b);
    }
    return "Odczytano tekst z obrazu";
}

std::string write_text_to_file()
{
    std::fstream output;
    output.open(lines[0].c_str(), std::ios::binary | std::ios::out);
    for(int i = 1; i < lines.size(); i++)
    {
        output.write(lines[i].c_str(), lines[i].size()+1);
    }
    output.close();
    return "Zapisano tekst do pliku tekstowego";
}

void code()
{
    std::string text_file = get_name_of_file("Prosze podac nazwe pliku tekstowego, w ktorym jest tekst do zaszyfrowania: ");
    std::string key_image_file = get_name_of_file("Prosze podac nazwe pliku obrazu, ktory posluzy jako klucz: ");
    std::string coded_image_file = get_name_of_file("Prosze podac nazwe pliku, w ktorym ma byc wygenerowany zaszyfrowany obraz: ");
    std::cout << load_text(text_file) << std::endl;
    std::cout << generate_coded_image(key_image_file, coded_image_file) << std::endl;
    std::cout << "Tekst z pliku " << text_file << " zostal zaszyfrowany w obrazie " << coded_image_file << std::endl;
    std::cout << std::endl;
}

void decode()
{
    std::string coded_image_file = get_name_of_file("Prosze podac nazwe pliku, w ktorym jest obraz z zaszyfrowanym tekstem: ");
    std::string key_image_file = get_name_of_file("Prosze podac nazwe pliku obrazu, ktory posluzy jako klucz: ");
    //std::string text_file = get_name_of_file("Prosze podac nazwe pliku tekstowego, w ktorym ma byc odszyfrowany tekst: ");
    std::cout << decode_image(coded_image_file, key_image_file) << std::endl;
    std::cout << write_text_to_file() << std::endl;
    std::cout << "Tekst z obrazu " << coded_image_file << " zostal odszyfrowany do pliku " << lines[0] << std::endl;
    std::cout << std::endl;
}

void test()
{
    std::cout << load_text("test.txt") << std::endl;
    std::cout << generate_coded_image("test_key.bmp", "test_coded.bmp") << std::endl;
    std::cout << decode_image("test_coded.bmp", "test_key.bmp") << std::endl;
    std::cout << write_text_to_file() << std::endl;
}

//graphics

void upline()
{
    std::cout << (char)(201);
    for(int i = 0; i < 60; i++)
        std::cout << (char)(205);
    std::cout << (char)(187) << std::endl;
}

void downline()
{
    std::cout << (char)(200);
    for(int i = 0; i < 60; i++)
        std::cout << (char)(205);
    std::cout << (char)(188) << std::endl;
    std::cout << std::endl;
}

void mline()
{
    std::cout << (char)(204);
    for(int i = 0; i < 60; i++)
        std::cout << (char)(205);
    std::cout << (char)(185) << std::endl;
}

void show_menu()
{
    upline();
    std::cout << (char)(186) << "\tMENU                                                 " << (char)(186) << std::endl;
    mline();
    std::cout << (char)(186) << "1\tZaszyfruj tekst z pliku do obrazu                    " << (char)(186) << std::endl;
    std::cout << (char)(186) << "2\tDeszyfruj tekst z obrazu do pliku                    " << (char)(186) << std::endl;
    std::cout << (char)(186) << "3\tPokaz wygenerowane cienie                            " << (char)(186) << std::endl;
    std::cout << (char)(186) << "4\tZakoncz program                                      " << (char)(186) << std::endl;
    mline();
}

void abouts()
{
    std::cout << "Program szyfrujacy i deszyfrujacy tekst za pomoca obrazow." << std::endl;
    std::cout << "Autor: Seweryn Panek, nr indeksu: 235398" << std::endl;
    std::cout << std::endl;
}

int main()
{
    abouts();
    int action;
    do
    {
        show_menu();
        std::cout << (char)(186) << "Akcja: ";
        std::cin >> action;
        downline();
        switch(action)
        {
        case 1:
            {
                code();
                break;
            }
        case 2:
            {
                decode();
                break;
            }
        case 3:
            {
                show_shadows();
                break;
            }
        case 4:
            {
                std::cout << "Dziekuje za skorzystanie z programu!" << std::endl;
                std::cout << "Milego dnia!" << std::endl;
                return 0;
                break;
            }
        case 5:
            {
                test();
                break;
            }
        default:
            {
                std::cin.clear();
                std::cin.sync();
                std::cout << "Nieprawidlowa akcja!" << std::endl;
                break;
            }
        }
    } while(true);
}
