🏧 ATM System — C++
A console-based ATM application built with C++. It simulates common ATM operations, including account login, quick and normal withdrawals, deposits, and balance inquiries.
The project uses local text-file storage, so no database server is required.
✨ Features
- 🔐 Account Login — Sign in using an account number and PIN.
- 💸 Quick Withdraw — Choose a preset amount: 20, 50, 100, 200, 400, 600, 800, or 1000.
- 💵 Normal Withdraw — Enter a withdrawal amount that is a positive multiple of 5.
- ➕ Deposit — Add money to the signed-in account after confirmation.
- 💰 Check Balance — View the current account balance.
- ✅ Transaction Validation — Reject invalid amounts and prevent withdrawals that exceed the available balance.
- 💾 File Handling — Save account details and updated balances in Clients.txt.
- 🧪 Demo Account Setup — Automatically creates a sample account if one does not already exist, while preserving existing client data.
🛠️ Technologies
- Language: C++
- Application: Console application
- Concepts: Structures, functions, enums, vectors, file I/O, string parsing, input validation, and menu-driven programming.
🚀 Getting Started
Requirements
- A C++ compiler
- Visual Studio 2022 on Windows, or another C++ development environment
Run with Visual Studio
1. Open ATMSystemWithC++.sln in Visual Studio.
2. Build the solution with Build → Build Solution or press Ctrl + Shift + B.
3. Run without debugging by pressing Ctrl + F5.
4. The program creates Clients.txt if needed and adds a demo account if it cannot find the existing demo account.
🔑 Demo Login
The application displays the demo credentials on the login screen.
- Account Number: 1001 in a fresh data file
- PIN: 1234
- Initial Balance: 5000.00
If account number 1001 is already assigned to another client, the program selects another unused account number and displays it on the login screen.
📁 Data Storage
Client information is stored in Clients.txt in the program's current working directory. Each record uses the following field order:
AccountNumber#//#Name#//#PinCode#//#Phone#//#AccountBalance
Example:
1001#//#ATM Demo Client#//#1234#//#01000000000#//#5000.000000
The delimiter is #//#. Keep the delimiter and field order consistent with the parsing and saving functions. The file is created relative to the application's working directory, which may differ from the source-code folder.
🔒 Security Note
This is an educational project, not a real banking system. PIN codes and account data are stored in plain text, and demo credentials are shown in the login screen. Use sample data only; do not store real customer or financial information in this project.
🎯 Project Goal
This project was created to practice C++ fundamentals by building an interactive ATM workflow with file persistence, transaction handling, and input validation.
Built as a learning project to practice C++ and file handling.
