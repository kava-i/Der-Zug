#include <iostream>
#include <fstream>
#include "CGameCreator.hpp"
#include "json.hpp"


int main()
{
    CGameCreator creator;
    std::cout << "Welcome to the game creator.\n";
    creator.selectWorld();
    
    int input = 9999;
    while(input != 0)
    {
        std::cout << "What do you want to do?\n";
        std::cout << "-- create -- \n";
        std::cout << "1. create character\n";
        std::cout << "2. create room\n";
        std::cout << "3. create item\n";
        std::cout << "4. create dialog\n";
        std::cout << "-- edit -- \n";
        std::cout << "5. edit character\n";
        std::cout << "6. edit room\n";
        std::cout << "7. edit item\n";
        std::cout << "8. edit dialog\n";
        std::cout << ">";
        std::cin >> input;
        std::cout << "You choose: " << input << std::endl;
    
        switch(input)
        {
            case 5: 
                creator.editCharacter(); 
                break;
            case 8: 
                creator.editDialog();
                break;
            default: 
                std::cout << "Invalid choice\n";
        }
    }    
    
    
}
