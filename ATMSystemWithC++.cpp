#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#include <cstdlib>

using namespace std;

const string ClientsFileName = "Clients.txt";
const string RecordSeparator = "#//#";
const string DemoClientName = "ATM Demo Client";
const string DefaultATMPinCode = "1234";
const double DefaultATMInitialBalance = 5000.0;

// The account number may be different if 1001 is already in use.
string DefaultATMAccountNumber;

enum enAtmOptions
{
    eQuickWithdraw = 1,
    eNormalWithdraw = 2,
    eDeposit = 3,
    eCheckBalance = 4,
    eLogout = 5
};

struct stClient
{
    string AccountNumber;
    string Name;
    string PinCode;
    string Phone;
    double AccountBalance = 0.0;
    bool MarkForDelete = false;
};

stClient CurrentClient;

// -------------------- General helpers --------------------

void ClearInputLine()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void ClearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Pause()
{
    cout << "\nPress Enter to continue...";
    ClearInputLine();
    cin.get();
}

string ReadText(const string& Prompt)
{
    string Value;
    cout << Prompt;
    getline(cin >> ws, Value);
    return Value;
}

int ReadChoice(const string& Prompt, int MinValue, int MaxValue)
{
    int Choice;
    while (true)
    {
        cout << Prompt;
        if (cin >> Choice && Choice >= MinValue && Choice <= MaxValue)
            return Choice;

        cout << "Invalid choice. Enter a number from "
            << MinValue << " to " << MaxValue << ".\n";
        ClearInputLine();
    }
}

bool Confirm(const string& Prompt)
{
    char Answer;
    while (true)
    {
        cout << Prompt << " (y/n): ";
        if (cin >> Answer)
        {
            if (Answer == 'y' || Answer == 'Y')
                return true;
            if (Answer == 'n' || Answer == 'N')
                return false;
        }

        cout << "Please enter y or n.\n";
        ClearInputLine();
    }
}

double ReadPositiveAmount(const string& Prompt)
{
    double Amount;
    while (true)
    {
        cout << Prompt;
        if (cin >> Amount && Amount > 0.0)
            return Amount;

        cout << "Please enter an amount greater than zero.\n";
        ClearInputLine();
    }
}

// -------------------- Record conversion and file handling --------------------

vector<string> SplitString(const string& Line, const string& Delimiter = RecordSeparator)
{
    vector<string> Parts;
    size_t Start = 0;
    size_t Position;

    while ((Position = Line.find(Delimiter, Start)) != string::npos)
    {
        Parts.push_back(Line.substr(Start, Position - Start));
        Start = Position + Delimiter.length();
    }

    Parts.push_back(Line.substr(Start));
    return Parts;
}

bool ConvertLineToRecord(const string& Line, stClient& Client)
{
    vector<string> Fields = SplitString(Line);
    if (Fields.size() != 5)
        return false;

    try
    {
        size_t ParsedLength = 0;
        double Balance = stod(Fields[4], &ParsedLength);
        if (ParsedLength != Fields[4].length())
            return false;

        Client.AccountNumber = Fields[0];
        Client.Name = Fields[1];
        Client.PinCode = Fields[2];
        Client.Phone = Fields[3];
        Client.AccountBalance = Balance;
        Client.MarkForDelete = false;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

string ConvertRecordToLine(const stClient& Client)
{
    return Client.AccountNumber + RecordSeparator +
        Client.Name + RecordSeparator +
        Client.PinCode + RecordSeparator +
        Client.Phone + RecordSeparator +
        to_string(Client.AccountBalance);
}

vector<stClient> LoadClientDataFromFile(const string& FileName = ClientsFileName)
{
    vector<stClient> Clients;
    ifstream MyFile(FileName);
    string Line;

    if (!MyFile.is_open())
        return Clients;

    while (getline(MyFile, Line))
    {
        if (Line.empty())
            continue;

        stClient Client;
        if (ConvertLineToRecord(Line, Client))
            Clients.push_back(Client);
        else
            cerr << "Warning: skipped an invalid record in " << FileName << ".\n";
    }

    return Clients;
}

bool SaveClientsDataToFile(const vector<stClient>& Clients,
    const string& FileName = ClientsFileName)
{
    ofstream MyFile(FileName, ios::out | ios::trunc);
    if (!MyFile.is_open())
    {
        cerr << "Error: could not open " << FileName << " for writing.\n";
        return false;
    }

    for (const stClient& Client : Clients)
    {
        if (!Client.MarkForDelete)
            MyFile << ConvertRecordToLine(Client) << '\n';
    }

    return MyFile.good();
}

int FindClientIndexByAccountNumber(const string& AccountNumber,
    const vector<stClient>& Clients)
{
    for (size_t i = 0; i < Clients.size(); ++i)
    {
        if (Clients[i].AccountNumber == AccountNumber)
            return static_cast<int>(i);
    }
    return -1;
}

// Creates one demo account on the first run, without overwriting existing data.
// If account 1001 is already taken, the next unused account number is selected.
bool EnsureDefaultATMClient()
{
    vector<stClient> Clients = LoadClientDataFromFile(ClientsFileName);

    // Reuse the demo account on later runs instead of creating a duplicate.
    for (const stClient& Client : Clients)
    {
        if (Client.Name == DemoClientName && Client.PinCode == DefaultATMPinCode)
        {
            DefaultATMAccountNumber = Client.AccountNumber;
            return true;
        }
    }

    int CandidateNumber = 1001;
    string CandidateAccountNumber;

    while (true)
    {
        CandidateAccountNumber = to_string(CandidateNumber);
        if (FindClientIndexByAccountNumber(CandidateAccountNumber, Clients) == -1)
            break;
        ++CandidateNumber;
    }

    stClient DemoClient;
    DemoClient.AccountNumber = CandidateAccountNumber;
    DemoClient.Name = DemoClientName;
    DemoClient.PinCode = DefaultATMPinCode;
    DemoClient.Phone = "01000000000";
    DemoClient.AccountBalance = DefaultATMInitialBalance;

    Clients.push_back(DemoClient);
    if (!SaveClientsDataToFile(Clients, ClientsFileName))
        return false;

    DefaultATMAccountNumber = DemoClient.AccountNumber;
    return true;
}

// -------------------- Client and transaction helpers --------------------

void PrintClientCard(const stClient& Client)
{
    cout << "\n----------------------------------------------\n";
    cout << "Account Number  : " << Client.AccountNumber << '\n';
    cout << "Name            : " << Client.Name << '\n';
    cout << "PIN Code        : " << Client.PinCode << '\n';
    cout << "Account Balance : " << fixed << setprecision(2)
        << Client.AccountBalance << '\n';
    cout << "----------------------------------------------\n";
}

bool LoadCurrentClientFromFile()
{
    vector<stClient> Clients = LoadClientDataFromFile();
    int Index = FindClientIndexByAccountNumber(CurrentClient.AccountNumber, Clients);

    if (Index == -1)
        return false;

    CurrentClient = Clients[Index];
    return true;
}

bool SaveTransaction(double Amount, bool IsDeposit)
{
    vector<stClient> Clients = LoadClientDataFromFile();
    int Index = FindClientIndexByAccountNumber(CurrentClient.AccountNumber, Clients);

    if (Index == -1)
    {
        cout << "Error: the current account could not be found.\n";
        return false;
    }

    if (!IsDeposit && Amount > Clients[Index].AccountBalance)
    {
        cout << "The amount exceeds your available balance.\n";
        return false;
    }

    if (!Confirm("Are you sure you want to perform this transaction?"))
    {
        cout << "Transaction cancelled.\n";
        return false;
    }

    if (IsDeposit)
        Clients[Index].AccountBalance += Amount;
    else
        Clients[Index].AccountBalance -= Amount;

    if (!SaveClientsDataToFile(Clients))
        return false;

    CurrentClient = Clients[Index];
    cout << "Transaction completed successfully. New balance: "
        << fixed << setprecision(2) << CurrentClient.AccountBalance << '\n';
    return true;
}

// -------------------- Deposit --------------------

void ShowDepositeScreen()
{
    ClearScreen();
    cout << "-------------------------------------\n";
    cout << "          Deposit Screen\n";
    cout << "-------------------------------------\n";

    if (!LoadCurrentClientFromFile())
    {
        cout << "Unable to load the current account.\n";
        return;
    }

    PrintClientCard(CurrentClient);
    double Amount = ReadPositiveAmount("Enter deposit amount: ");
    SaveTransaction(Amount, true);
}

// -------------------- Normal withdrawal --------------------

int ReadNormalWithdraw()
{
    int Amount;
    while (true)
    {
        cout << "Enter a withdrawal amount that is a multiple of 5: ";
        if (cin >> Amount && Amount > 0 && Amount % 5 == 0)
            return Amount;

        cout << "Amount must be a positive multiple of 5.\n";
        ClearInputLine();
    }
}

void ShowNormalWithdraw()
{
    ClearScreen();
    cout << "--------------------------------------------\n";
    cout << "         Normal Withdrawal Screen\n";
    cout << "--------------------------------------------\n";

    if (!LoadCurrentClientFromFile())
    {
        cout << "Unable to load the current account.\n";
        return;
    }

    PrintClientCard(CurrentClient);
    int Amount = ReadNormalWithdraw();
    SaveTransaction(static_cast<double>(Amount), false);
}

// -------------------- Quick withdrawal --------------------

short ReadQuickWithdrawOption()
{
    return static_cast<short>(ReadChoice("Choose an option [1-9]: ", 1, 9));
}

int GetQuickWithdrawAmount(short Option)
{
    switch (Option)
    {
    case 1: return 20;
    case 2: return 50;
    case 3: return 100;
    case 4: return 200;
    case 5: return 400;
    case 6: return 600;
    case 7: return 800;
    case 8: return 1000;
    default: return 0; // Option 9 returns to the ATM menu.
    }
}

void ShowQuickWithDraw()
{
    ClearScreen();
    cout << "-----------------------------------------\n";
    cout << "             Quick Withdraw\n";
    cout << "-----------------------------------------\n";
    cout << "[1] 20          [2] 50\n";
    cout << "[3] 100         [4] 200\n";
    cout << "[5] 400         [6] 600\n";
    cout << "[7] 800         [8] 1000\n";
    cout << "[9] Back to ATM menu\n";
    cout << "-----------------------------------------\n";

    if (!LoadCurrentClientFromFile())
    {
        cout << "Unable to load the current account.\n";
        return;
    }

    cout << "Your balance is: " << fixed << setprecision(2)
        << CurrentClient.AccountBalance << '\n';

    short Option = ReadQuickWithdrawOption();
    if (Option == 9)
        return;

    int Amount = GetQuickWithdrawAmount(Option);
    SaveTransaction(static_cast<double>(Amount), false);
}

// -------------------- Balance screen --------------------

void ShowCheckBalanceScreen()
{
    ClearScreen();
    cout << "------------------------------------\n";
    cout << "         Check Balance Screen\n";
    cout << "------------------------------------\n";

    if (!LoadCurrentClientFromFile())
    {
        cout << "Unable to load the current account.\n";
        return;
    }

    cout << "Account Number: " << CurrentClient.AccountNumber << '\n';
    cout << "Your balance is: " << fixed << setprecision(2)
        << CurrentClient.AccountBalance << '\n';
}

// -------------------- Login and ATM menu --------------------

bool FindClientByAccountNumberAndPinCode(const string& AccountNumber,
    const string& PinCode,
    stClient& Client)
{
    vector<stClient> Clients = LoadClientDataFromFile(ClientsFileName);

    for (const stClient& ExistingClient : Clients)
    {
        if (ExistingClient.AccountNumber == AccountNumber &&
            ExistingClient.PinCode == PinCode)
        {
            Client = ExistingClient;
            return true;
        }
    }

    return false;
}

bool LoadUserInfo(const string& AccountNumber, const string& PinCode)
{
    return FindClientByAccountNumberAndPinCode(AccountNumber, PinCode, CurrentClient);
}

bool Login()
{
    while (true)
    {
        ClearScreen();
        cout << "================================================\n";
        cout << "                 ATM Login Screen\n";
        cout << "================================================\n";
        cout << "Demo account credentials (for testing only):\n";
        cout << "Account Number: " << DefaultATMAccountNumber << '\n';
        cout << "PIN Code      : " << DefaultATMPinCode << '\n';
        cout << "------------------------------------------------\n";

        string AccountNumber = ReadText("Enter Account Number: ");
        string PinCode = ReadText("Enter PIN Code: ");

        if (LoadUserInfo(AccountNumber, PinCode))
        {
            cout << "\nLogin successful. Welcome, " << CurrentClient.Name << ".\n";
            return true;
        }

        cout << "\nInvalid account number or PIN code. Please try again.";
        cout << "\nPress Enter to retry...";
        string Dummy;
        getline(cin, Dummy);
    }
}

void ShowATMMenue()
{
    while (true)
    {
        ClearScreen();
        cout << "===========================================\n";
        cout << "              ATM Main Menu\n";
        cout << "===========================================\n";
        cout << "[1] Quick Withdraw\n";
        cout << "[2] Normal Withdraw\n";
        cout << "[3] Deposit\n";
        cout << "[4] Check Balance\n";
        cout << "[5] Logout\n";
        cout << "===========================================\n";

        int Choice = ReadChoice("Choose an option [1-5]: ", 1, 5);
        switch (Choice)
        {
        case eQuickWithdraw:
            ShowQuickWithDraw();
            Pause();
            break;
        case eNormalWithdraw:
            ShowNormalWithdraw();
            Pause();
            break;
        case eDeposit:
            ShowDepositeScreen();
            Pause();
            break;
        case eCheckBalance:
            ShowCheckBalanceScreen();
            Pause();
            break;
        case eLogout:
            CurrentClient = stClient();
            return;
        }
    }
}

int main()
{
    // Ensure the text file exists before loading any client records.
    ofstream CreateFileIfMissing(ClientsFileName, ios::app);
    if (!CreateFileIfMissing.is_open())
    {
        cerr << "Error: could not create or open " << ClientsFileName << ".\n";
        return 1;
    }
    CreateFileIfMissing.close();

    // Create the demo account once if it does not already exist.
    if (!EnsureDefaultATMClient())
    {
        cerr << "Error: could not create or save the default ATM account.\n";
        return 1;
    }

    // Logging out returns to the login screen.
    while (true)
    {
        Login();
        ShowATMMenue();
    }

    return 0;
}
