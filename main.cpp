#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include <iomanip>  // For formatting
#include <cstdlib>  // For system("clear") or system("cls") depending on the OS
#include<limits>

// ANSI color codes
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
//User_cred Path
#define USER_CRED "user_cred.csv"

using namespace std;

// function to clear the screen 
void clearScreen() {
    #ifdef _WIN32
    system("cls");
    
    #else
        system("clear");
    #endif
}


int getValidOption()
{
    int option;
    while(true)
    {
        cout<<GREEN<<"Please, Enter a valid option : ";
        cin>>option;
        cout<<RESET;

        // option input validation check if digit
        if(cin.fail())
        {
            cout <<RED<< "Invalid input. Please enter a valid integer." <<endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else{
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return option;
        }
    }
}
void enterToContinue() {
    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

class User{
    private:
        string name;
        string accountNumber;
        string pin;
        double balance;

    public:
        User(string name,string accountNumber, string pin, double balance) : name(name), accountNumber(accountNumber),
        pin(pin), balance(balance){};

        bool checkPin(string pinNumber)
        {
            return pin == pinNumber;
        }

        void deposit(double amount)
        {
            balance+=amount;
            cout<< "Your new Balance is: " << balance <<endl;
        }

        bool withdraw(double amount)
        {
            if(amount <=balance)
            {
                balance-=amount;
                cout<< "Withdrawal Successful! Your new Balance is: " << balance <<endl;
                return true;
            }
            else{
                cout<<"Insufficient Balance" <<endl;
                return false;
            }
        }

        double getBalance()
        {
            return balance;
        }

        bool changePin(const string& pinNumber){
            pin = pinNumber;
            cout<< "PIN Changed Successfully" <<endl;
            return true;
        }

        string getName(){
            return name;
        }

        string getAccountNumber()
        {
            return accountNumber;
        }

        string getPin()
        {
            return pin;
        }
};

class ATM{
    private:
        vector<User> users;
        User* currentUser = nullptr;
    public:
        ATM(const std::string& filename) {
            loadUsersFromFile(filename);
        }

        void loadUsersFromFile(const std::string& filename) {
            ifstream file(filename);
            if (!file) {
                cerr << "Error opening file: " << filename << endl;
                return;
            }

            string line;
            // Skip the first header line
            getline(file, line);

            while (getline(file, line)) {
                stringstream ss(line);
                string name, accountNumber, pin, balanceStr;
                double balance;

                getline(ss, name, ',');
                getline(ss, accountNumber, ',');
                getline(ss, pin, ',');
                getline(ss, balanceStr, ',');

                // Trim any extra whitespace (optional)
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                accountNumber.erase(0, accountNumber.find_first_not_of(" \t"));
                accountNumber.erase(accountNumber.find_last_not_of(" \t") + 1);
                pin.erase(0, pin.find_first_not_of(" \t"));
                pin.erase(pin.find_last_not_of(" \t") + 1);
                balanceStr.erase(0, balanceStr.find_first_not_of(" \t"));
                balanceStr.erase(balanceStr.find_last_not_of(" \t") + 1);

                try {
                    balance = stod(balanceStr);
                } catch (const invalid_argument& e) {
                    cerr << "Invalid balance format: " << balanceStr << " in line: " << line << std::endl;
                    continue;
                }

                users.push_back(User(name, accountNumber, pin, balance));
            }

            file.close();
        }       

        User* findUser(const string& accountNumber){
            for(auto& user: users)
            {
                if(user.getAccountNumber() == accountNumber)
                    return &user;
            }
            return nullptr;
        }
        
        bool login(const string& accountNumber, const string& pinNumber)
        {
            currentUser = findUser(accountNumber);
            if(currentUser && currentUser->checkPin(pinNumber))
            {
                cout<< "LOGIN SUCCESSFUL. Welcome, " << currentUser->getName() << "!" << endl;
                return true;
            }
            else{
                cout<< "Login Failed! Incorrect Account Number or PIN" <<endl;
                return false;
            }
        }

        void updateFile()
        {
            ofstream file(USER_CRED);
            if (!file) {
                cerr << "Error opening file: " << "user_cred.txt" << endl;
                return;
            }
            file << "Name,Account Number,PIN,Balance\n";
            for(auto& user: users)
            {
                file << user.getName() << "," << user.getAccountNumber() << "," << user.getPin() << "," << user.getBalance() << "\n";
            }
            file.close();
        }
        
        void logout()
        {
            currentUser = nullptr;
            std::cout << "Logged Out Successfully!"<<endl;
        }
        
        void performTransaction() {
            if (!currentUser) {
                cout << RED << "No User Logged in!" << RESET << endl;
                return;
            }

            int choice;
            do {
                clearScreen();

                // Calculate the lengths for proper alignment
                string name = currentUser->getName();
                string accountNumber = currentUser->getAccountNumber();
                double balance = currentUser->getBalance();
                string balanceStr = "$" + to_string(balance);

                // Ensure the widths match the box length
                int boxWidth = 61;
                int nameWidth = boxWidth - 16 - 18; // Adjust the padding accordingly
                int accountWidth = 20;
                int balanceStrLength = balanceStr.length();
                int balancePadding = boxWidth - 16 - balanceStrLength - 3; // -3 for the spaces and | at the end

                // Display user information in a boxed layout with colors
                cout << CYAN << "+-----------------------------------------------------------+" << RESET << endl;
                cout << CYAN << "| " << RESET << left << setw(nameWidth) << ("Name: " + name)
                    << CYAN << "| " << RESET << setw(accountWidth) << ("Account Number: " + accountNumber) << CYAN << " |\n";
                cout << CYAN << "+-----------------------------------------------------------+" << RESET << endl;
                cout << CYAN << "| " << RESET << "Current Balance:" 
                    << setw(balancePadding-1) << " " << YELLOW << balanceStr << RESET << CYAN << " |\n";
                cout << CYAN << "+-----------------------------------------------------------+" << RESET << endl;

                // Display the ATM menu
                cout << GREEN << "\nATM MENU:\n" << RESET;
                cout << "1. Deposit Money\n";
                cout << "2. Withdraw Money\n";
                cout << "3. Change PIN\n";
                cout << "4. Logout\n";
                choice = getValidOption();

                // Handle the user's choice
                switch (choice) {
                    case 1: {
                        double amount;
                        cout << "\nEnter the amount to deposit: ";
                        cin >> amount;
                        currentUser->deposit(amount);
                        enterToContinue();
                        updateFile();
                        break;
                    }
                    case 2: {
                        double amount;
                        cout << "\nEnter the amount to withdraw: ";
                        cin >> amount;
                        currentUser->withdraw(amount);
                        updateFile();
                        enterToContinue();
                        break;
                    }
                    case 3: {
                        string newPin;
                        cout << "\nEnter new PIN: ";
                        cin >> newPin;
                        currentUser->changePin(newPin);
                        enterToContinue();
                        updateFile();
                        break;
                    }
                    case 4:
                        logout();
                        enterToContinue();
                        break;
                    default:
                        cout << RED << "\nInvalid Choice. Please Try Again." << RESET << endl;
                        enterToContinue();
                }

            } while (choice != 4);
        
        }

        void getUsers()
        {
            for(auto& user: users)
            {
                cout<< user.getName() << endl;
            }
        }

        bool userExists(const string& accountNumber){
            for(auto& user: users)
            {
                if(user.getAccountNumber() == accountNumber)
                    return true;
            }
        }

        void addUser(const User& user)
        {
            users.push_back(user);
            updateFile();
            cout<< GREEN<< "User added Successfully!" <<endl;
        }
};

bool isPinValid(const string& pinNumber, const string& confirmPIN)
{
    if(pinNumber.length() !=4)
    {
        cout<< RED << "The  PIN should contain 4 Digits!" <<endl << RESET;
        return false;
    }   
    else if(pinNumber != confirmPIN)
    {
        cout<<RED<<"PIN didn't match! Please try again." <<endl << RESET;
        return false;
    }
    return true;
}

string generateAccountNumber()
{
    string accNum;
    for (int i = 0; i < 12; i++)
    {
        accNum += to_string(rand() % 10);
    }
    return accNum;
}

void login()
{
    ATM atm(USER_CRED);
    string accountNumber, pin;
    cout<< RED << "Enter Account Number: ";
    cin >> accountNumber;
    cout<< RED << "Enter PIN: ";
    cin >> pin;
    cout<<RESET;

    if(atm.login(accountNumber, pin))
    {
        atm.performTransaction();
    }
}

void userRegister(){
    ATM atm(USER_CRED);
    string name, accountNumber, pinNumber, confirmPin;
    double balance;
    cout<< "Enter the name of the user: ";
    cin.ignore();
    getline(cin, name);
    
    //generate unique account number
    do{
        accountNumber = generateAccountNumber();
    }while(atm.userExists(accountNumber));

    do{
        cout<< "Enter the PIN: ";
        cin >> pinNumber;

        cout<< "Re-enter the PIN: ";
        cin >> confirmPin;
    }while(!isPinValid(pinNumber,confirmPin));


    cout << "Enter the balance: ";
    cin >> balance;

    User user(name, accountNumber, pinNumber, balance);
    atm.addUser(user);
    clearScreen();

    string balanceStr = "$" + to_string(balance);

     // Ensure the widths match the box length
    int boxWidth = 61;
    int nameWidth = boxWidth - 16 - 18; // Adjust the padding accordingly
    int accountWidth = 20;
    int balanceStrLength = balanceStr.length();
    int balancePadding = boxWidth - 16 - balanceStrLength - 3; // -3 for the spaces and | at the end

    // Display user information in a boxed layout with colors
    cout << CYAN << "+-----------------------------------------------------------+" << RESET << endl;
    cout << CYAN << "| " << RESET << left << setw(nameWidth) << ("Name: " + name)
        << CYAN << "| " << RESET << setw(accountWidth) << ("Account Number: " + accountNumber) << CYAN << " |\n";
    cout << CYAN << "+-----------------------------------------------------------+" << RESET << endl;
    cout << CYAN << "| " << RESET << "Current Balance:" 
        << setw(balancePadding-1) << " " << YELLOW << balanceStr << RESET << CYAN << " |\n";
    cout << CYAN << "+-----------------------------------------------------------+" << RESET << endl;

    cout<<GREEN<<"\n\nSuccessfully Created Account! Please note the credentials."<<endl;
    enterToContinue();
}




int main()
{
    int option;
    do
    {
        cout<<RESET;
        clearScreen();
        cout<< "Please Select an Option : " <<endl;
        cout<< "1. Login" <<endl;
        cout<< "2. Register" <<endl;
        cout<< "3. End Program"<<endl;
        option = getValidOption();

    
        switch(option)
        {
            case 1:
                login();
                break;
            case 2:
                userRegister();
                break;
            case 3:
                cout<<"Thank you for visiting us! Press enter to ."<<endl;
                enterToContinue();
                break;
            default:
                cout<<"Invalid Choice! Please Try Again."<<endl;
                break;
        }
    }while(option!=3);
    return 0;
}
