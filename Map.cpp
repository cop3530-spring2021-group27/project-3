#include <stdio.h>      
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <random>
#include <string> 

#include "fort.hpp" //library for printing our table in a nice manner

using namespace std;

struct Patient//student node to pass around
{
	int numberID; //patient id stored as an int such that we do not have to use stoi() at all 
	string patientID; //the patient ID number, stored in a string so you don't lose 0s when printing
	string patientName; 
	string patientBday;
	int priorityValue;
	int ddimerValue;
	int sodiumLevel;
	int potassiumLevel;
	int height;
	Patient* left = nullptr;
	Patient* right = nullptr;

	Patient(int numID, string ID, string name, string bDay, int priorityValue_, int ddimerValue_, int sodiumLevel_, int potassiumLevel_, int height_, Patient* left_, Patient* right_) 
	{
		numberID = numID;
		patientID = ID;
		patientName = name;
		patientBday = bDay;
		priorityValue = priorityValue_;
		ddimerValue = ddimerValue_;
		sodiumLevel = sodiumLevel_;
		potassiumLevel = potassiumLevel_;
		height = height_;
		left = left_;
		right = right_;
	}
};

/*
Note: The implementation of this AVLTree class has much of the code reused from the project 1 submission of Eric Shaw.
Namely, the private functions are identical except for variable names, and most of the public functions are modified from
their original implementation to fit the needs of this project.
*/ 
class AVLTree {

public:

	AVLTree();
	Patient* getRoot();
	void setRoot(Patient* root);

	Patient* insertNode(Patient* root, int numID, std::string ID, std::string name, std::string bDay, int priorityValue_, int ddimerValue_, int sodiumLevel_, int potassiumLevel_);		
	Patient* searchID(Patient* root, int id);

	void traverse(vector<Patient*>& patients);
	void traverse(Patient* root, vector<Patient*>& patients);
	void findPatientName(Patient* root, vector<Patient*>& patients, string patientName);

private:

	Patient* root;
	Patient* fixAVLTree(Patient* root);
	Patient* rotateRight(Patient* node);
	Patient* rotateLeft(Patient* node);
	Patient* rotateLeftRight(Patient* node);
	Patient* rotateRightLeft(Patient* node);
	int getBalanceFactor(Patient* root);
	int getHeight(Patient* root);

};

AVLTree::AVLTree() 
{
	this->root = NULL; //the root of the tree starts as NULL
}

Patient* AVLTree::getRoot() //getter for our private root variable
{
	return root;
}

void AVLTree::setRoot(Patient* root) //setter for our private root variable
{
	this->root = root;
}


Patient* AVLTree::insertNode(Patient* root, int numID, std::string ID, std::string name, std::string bDay, int priorityValue_, int ddimerValue_, int sodiumLevel_, int potassiumLevel_) 
{
	if (root == NULL) //this is the first node being inserted	
	{
		Patient* newRoot = new Patient(numID, ID, name, bDay, priorityValue_, ddimerValue_, sodiumLevel_, potassiumLevel_, 0, NULL, NULL);
		return newRoot;
	}
	else if (numID > root->numberID) //id being passsed in is lower than the id of the current node	
	{
		if (root->right == NULL) //the right child is NULL, so we just add it on	
		{
			root->right = new Patient(numID, ID, name, bDay, priorityValue_, ddimerValue_, sodiumLevel_, potassiumLevel_, 0, NULL, NULL);
		}
		else
			root->right = insertNode(root->right, numID, ID, name, bDay, priorityValue_, ddimerValue_, sodiumLevel_, potassiumLevel_);
	}
	else if (numID < root->numberID) //id being passed in is lower than the id of the current node	
	{
		if (root->left == NULL) //the left child is NULL, so we just add it on	
		{
			root->left = new Patient(numID, ID, name, bDay, priorityValue_, ddimerValue_, sodiumLevel_, potassiumLevel_, 0, NULL, NULL);
		}
		else
			root->left = insertNode(root->right, numID, ID, name, bDay, priorityValue_, ddimerValue_, sodiumLevel_, potassiumLevel_);
	}
	else //id is a duplicate, hence unsuccessful	
	{
		cout << "Troubleshooting: the ID you tried to insert is already in the map (i think? this error popped up more than it should have in testing)" << endl; //this should never print in our implementation
		return root;
	}
	root->height = getHeight(root); //update the height	
	int balanceFactor = getBalanceFactor(root);
	if (balanceFactor > 1 || balanceFactor < -1) //imbalance is present	
	{
		root = fixAVLTree(root);
		root->height = getHeight(root); //update the height (necessary in case a rotation occurs)	
	}
	root->height = getHeight(root);
	return root;
}

Patient* AVLTree::searchID(Patient* root, int ID) 
{
	Patient* temp = NULL;
	if (root == NULL) 
	{
		return NULL;
	}
	else if (ID == root->numberID) //we have found the ID we are looking for	
	{
		return root;
	}
	else if (ID > root->numberID) //the ID is in the right subtree (if it exists)	
	{
		if (root->right == NULL)
			return NULL;
		temp = searchID(root->right, ID);
	}
	else if (ID < root->numberID)//the ID is in the left subtree (if it exists)	
	{
		if (root->left == NULL)
			return NULL;
		temp = searchID(root->left, ID);
	}
	return temp;
}

void AVLTree::traverse(vector<Patient*>& patients) 
{
	if (patients.size() > 0) // we have already traversed the map, no need to do it again
	{
		return;
	}
	traverse(getRoot(), patients);

}

void AVLTree::traverse(Patient* root, vector<Patient*>& patients) //patients will be a vector of all patients (also is inorder, so sorted by ID already)
{
	if (root != nullptr && root->left != nullptr)
		traverse(root->left, patients);
	if (root != nullptr)
		patients.push_back(root);
	if (root != nullptr && root->right != nullptr)
		traverse(root->right, patients);
}

void AVLTree::findPatientName(Patient* root, vector<Patient*>& patients, string patientName_) //patients will be a vector of every patient with the string patientName_
{
	vector<Patient*> patientList;
	traverse(patientList);

	for (unsigned int i = 0; i < patientList.size(); i++) 
	{
		Patient* p = patientList[i];
		if (p->patientName == patientName_) 
		{
			patients.push_back(p);
		}
	}
}
//private functions start here

Patient* AVLTree::fixAVLTree(Patient* node) 
{
	//note that these definitions may appear "backwards" since we define balanceFactor = right - left height
	if (getBalanceFactor(node) > 1) //parent balance factor is 2
	{
		if (getBalanceFactor(node->right) >= 1) 
		{
			node = rotateLeft(node);
		}
		else if (getBalanceFactor(node->right) <= -1) 
		{
			node = rotateRightLeft(node);
		}
	}
	else //balance factor is -2 (or less)
	{
		if (getBalanceFactor(node->left) >= 1) 
		{
			node = rotateLeftRight(node);
		}
		else if (getBalanceFactor(node->left) <= -1) 
		{
			node = rotateRight(node);
		}
	}
	return node;
}

//4 standard rotations below, code taken from my own stepik exercises 

Patient* AVLTree::rotateLeft(Patient* node) 
{
	Patient* grandchild = node->right->left;
	Patient* newParent = node->right;
	newParent->left = node;
	node->right = grandchild;
	return newParent;
}

Patient* AVLTree::rotateRight(Patient* node) 
{
	Patient* grandchild = node->left->right;
	Patient* newParent = node->left;
	newParent->right = node;
	node->left = grandchild;
	return newParent;
}

Patient* AVLTree::rotateLeftRight(Patient* node) 
{
	Patient* newParent1 = node->left;
	node->left = rotateLeft(newParent1);
	return rotateRight(node);
}

Patient* AVLTree::rotateRightLeft(Patient* node) 
{
	Patient* newParent1 = node->right;
	node->right = rotateRight(newParent1);
	return rotateLeft(node);
}

int AVLTree::getBalanceFactor(Patient* root) 
{
	int leftHeight = -1; //height of left subtree
	int rightHeight = -1; //height of right subtree

	if (root->right != NULL)
		rightHeight = getHeight(root->right);
	if (root->left != NULL)
		leftHeight = getHeight(root->left);

	int balanceFactor;
	balanceFactor = rightHeight - leftHeight;
	//note that this is defined backwards from what the "typical" definition is. changing it would require a lot of functions changing, so I left it

	return balanceFactor;
}

int AVLTree::getHeight(Patient* root) 
{
	if (root->right == NULL && root->left == NULL) //the node is a leaf
	{
		return 0;
	}

	int height = 0;

	int leftHeight;
	int rightHeight;

	if (root->right == NULL) 
	{
		rightHeight = 0;
	}
	else
		rightHeight = root->right->height;
	if (root->left == NULL) 
	{
		leftHeight = 0;
	}
	else
		leftHeight = root->left->height;

	if (rightHeight >= leftHeight) //we want to add 1 plus the height of the larger subtree
	{
		height = 1 + rightHeight;
	}
	else if (rightHeight < leftHeight) 
	{
		height = 1 + leftHeight;
	}

	return height;
}


class Map {
public:

	int size; //amount of patient nodes in the tree

	Map() 
	{
		size = 0;
	}

	void insert(int numID, std::string ID, std::string name, std::string bDay, int priorityValue_, int ddimerValue_, int sodiumLevel_, int potassiumLevel_)  //Puts Patient node into the map
	{
		size++;
		t.setRoot(t.insertNode(t.getRoot(), numID, ID, name, bDay, priorityValue_, ddimerValue_, sodiumLevel_, potassiumLevel_));
	}

	Patient* at(int index) //same as [], returns Patient with specified PatientID
	{
		return t.searchID(t.getRoot(), index);
	}

	Patient* operator [](int index) //same as at, returns Patient with the specified PatientID
	{
		return t.searchID(t.getRoot(), index);
	}

	bool empty() //returns if the map is empty
	{
		if (size == 0)
			return true;
		else
			return false;
	}

	void traverseCall(vector<Patient*>& patients) //patients will be a vector of all patients (also is inorder, so sorted by ID already)
	{
		t.traverse(patients);
	}

	void findPatientNameCall(vector<Patient*>& patients, string patientName_) //patients will be a vector of every patient with the string patientName_
	{
		t.findPatientName(t.getRoot(), patients, patientName_);
	}

	void sort(vector<Patient*>& patients, string sortType)  //helper for calling merge sort 
	{
		sort(0, patients.size() - 1, patients, sortType);
	}

	void sort(int begin, int end, vector<Patient*>& patients, string sortType)  //standard merge sort call
	{
		if (begin < end)  // ensures we don't infinitely loop
		{
			sort(begin, (begin + end) / 2, patients, sortType);
			sort((begin + end) / 2 + 1, end, patients, sortType);
			merge(begin, (begin + end) / 2, end, patients, sortType);
		}
	}

	void merge(int begin, int mid, int end, vector<Patient*>& patients, string sortType) 
	{
		vector<Patient*> firstHalf;
		vector<Patient*> secondHalf;

		for (int i = 0; i < mid - begin + 1; i++) //creates first half vector
		{
			firstHalf.push_back(patients[begin + i]);
		}

		for (int i = 0; i < end - mid; i++) //creates second half vector 
		{
			secondHalf.push_back(patients[mid + 1 + i]);
		}

		int firstHalfTracker = 0;
		int secondHalfTracker = 0;
		int patientsTracker = begin;

		while (firstHalfTracker < mid - begin + 1 && secondHalfTracker < end - mid) 
		{
			if (sortType == "name") //sorts alphabetically
			{
				string fhPatientName = firstHalf[firstHalfTracker]->patientName;
				string shPatientName = secondHalf[secondHalfTracker]->patientName;

				unsigned int i = 0;
				bool fhPatientFirst = false; // Whether the first half's patient's name is alphabetically before or after the second half's patient's name.
				
				while (i < fhPatientName.length() && i < shPatientName.length()) 
				{
					if (fhPatientName[i] < shPatientName[i]) 
					{
						fhPatientFirst = true;
						break;
					}
					else if (fhPatientName[i] > shPatientName[i]) 
					{
						break;
					}
					i++;
				}

				if (fhPatientFirst) 
				{
					patients[patientsTracker++] = firstHalf[firstHalfTracker++];
				}
				else 
				{
					patients[patientsTracker++] = secondHalf[secondHalfTracker++];
				}
			}

			else if (sortType == "id") //sorts from low to high
			{
				if (firstHalf[firstHalfTracker]->numberID <= secondHalf[secondHalfTracker]->numberID) 
				{
					patients[patientsTracker++] = firstHalf[firstHalfTracker++];
				}
				else {
					patients[patientsTracker++] = secondHalf[secondHalfTracker++];
				}
			}

			else if (sortType == "bday") //sorts from oldest to youngest
			{
				string fhBday = firstHalf[firstHalfTracker]->patientBday;
				string shBday = secondHalf[secondHalfTracker]->patientBday;
				int fhYear = stoi(fhBday.substr(6, 4)); 
				int shYear = stoi(shBday.substr(6, 4)); 
				
				if (fhYear < shYear) 
				{
					patients[patientsTracker++] = firstHalf[firstHalfTracker++];
				}
				else if (fhYear > shYear) 
				{
					patients[patientsTracker++] = secondHalf[secondHalfTracker++];
				}
				
				else 
				{
					int fhMonth = stoi(fhBday.substr(0, 2));
					int shMonth = stoi(shBday.substr(0, 2));

					if (fhMonth < shMonth) 
					{
						patients[patientsTracker++] = firstHalf[firstHalfTracker++];
					}
					else if (fhMonth > shMonth) 
					{
						patients[patientsTracker++] = secondHalf[secondHalfTracker++];
					}

					else 
					{
						int fhDay = stoi(fhBday.substr(3, 2));
						int shDay = stoi(shBday.substr(3, 2));

						if (fhDay < shDay) 
						{
							patients[patientsTracker++] = firstHalf[firstHalfTracker++];
						}
						else if (fhDay >= shDay) 
						{
							patients[patientsTracker++] = secondHalf[secondHalfTracker++];
						}

					}

				}
			}

			else if (sortType == "ddimer") //sorts from high to low
			{
				if (firstHalf[firstHalfTracker]->ddimerValue >= secondHalf[secondHalfTracker]->ddimerValue) 
				{
					patients[patientsTracker++] = firstHalf[firstHalfTracker++];
				}
				else 
				{
					patients[patientsTracker++] = secondHalf[secondHalfTracker++];
				}
			}

			else if (sortType == "sodium") //sorts from high to low
			{
				if (firstHalf[firstHalfTracker]->sodiumLevel >= secondHalf[secondHalfTracker]->sodiumLevel) 
				{
					patients[patientsTracker++] = firstHalf[firstHalfTracker++];
				}
				else 
				{
					patients[patientsTracker++] = secondHalf[secondHalfTracker++];
				}
			}

			else if (sortType == "potassium") //sorts from high to low
			{
				if (firstHalf[firstHalfTracker]->potassiumLevel >= secondHalf[secondHalfTracker]->potassiumLevel) 
				{
					patients[patientsTracker++] = firstHalf[firstHalfTracker++];
				}
				else 
				{
					patients[patientsTracker++] = secondHalf[secondHalfTracker++];
				}
			}


			else if (sortType == "priority") //sorts from high to low 
			{
				if (firstHalf[firstHalfTracker]->priorityValue <= secondHalf[secondHalfTracker]->priorityValue) 
				{
					patients[patientsTracker++] = firstHalf[firstHalfTracker++];
				}
				else 
				{
					patients[patientsTracker++] = secondHalf[secondHalfTracker++];
				}
			}

		}

		while (firstHalfTracker < mid - begin + 1) // copies rest of first vector if necessary
		{
			patients[patientsTracker++] = firstHalf[firstHalfTracker++];
		}

		while (secondHalfTracker < end - mid) // copies rest of second vector if necessary
		{
			patients[patientsTracker++] = secondHalf[secondHalfTracker++];
		}
	}


private:
	AVLTree t; //our ordered map is backed by an AVL Tree
};

int main() 
{
	cout << "Loading..." << endl; 
	Map m;
	string command;

	//placeholders for reading from the file
	int numID = 99999999;
	string stringNumID = "";
	string junkHolder = "";
	string birthDay = "";
	string nameID = "";
	string dDimerLevel = "";
	int numDdimer = 200;
	string sodiumLevel = "";
	int numSodium = 140;
	string potassiumLevel = "";
	int numPotassium = 40;
	string priorityHolder = "";
	int numberPriority = 0;


	ifstream fileRead;
	fileRead.open("Data.csv"); //name of the file with our data

	if (fileRead.is_open()) 
	{
		char test[4] = { 0 };
		fileRead.read(test, 3);

		for (int i = 0; i < 100000; i++) 
		{
			birthDay = "";
			potassiumLevel = "";
			getline(fileRead, nameID, ',');

			getline(fileRead, junkHolder, ',');

			getline(fileRead, junkHolder, ',');

			getline(fileRead, stringNumID, ',');
			numID = stoi(stringNumID);
			while (stringNumID.length() < 6) 
			{
				stringNumID = "0" + stringNumID;
			}

			getline(fileRead, junkHolder, ',');
			if (junkHolder.length() == 1) 
				junkHolder = "0" + junkHolder;
			birthDay.append(junkHolder);
			birthDay.append("/");

			getline(fileRead, junkHolder, ',');
			if (junkHolder.length() == 1) 
				junkHolder = "0" + junkHolder;
			birthDay.append(junkHolder);
			birthDay.append("/");

			getline(fileRead, junkHolder, ',');
			birthDay.append(junkHolder);

			getline(fileRead, dDimerLevel, ',');
			numDdimer = stoi(dDimerLevel);

			getline(fileRead, sodiumLevel, ',');
			numSodium = stoi(sodiumLevel);

			getline(fileRead, potassiumLevel, ',');
			numPotassium = stoi(potassiumLevel);

			getline(fileRead, junkHolder, ',');
			getline(fileRead, junkHolder, ',');
			getline(fileRead, junkHolder, ',');
			getline(fileRead, priorityHolder);
			numberPriority = stol(priorityHolder);

			m.insert(numID, stringNumID, nameID, birthDay, numberPriority, numDdimer, numPotassium, numSodium);
		}
		fileRead.close();
	}

	vector<Patient*> patients;
	m.traverseCall(patients);
	cout << endl << "Load complete." << endl << endl;

	cout << "Output to:\n\tfile\tOR\tterminal" << endl;
	getline(cin, command);
	ofstream file("output.txt");
	bool fileOut = false;
	if (command == "file") {
		fileOut = true;
		cout << "Outputting to ./output.txt" << endl;
	}

	cout << "Options: " << endl << endl << "    sort - Sorts by one of the following: " << endl << "        id    name    bday    priority    ddimer    sodium    potassium" << endl << endl<< "    display - Displays a patient's details given their name or id" << endl << endl << "    exit - Exits this program" << endl << endl;	while (true) //runs until we break
	{
		getline(cin, command);

		int firstSpace = -1;

		firstSpace = command.find(" ", 0);

		string firstWord = command.substr(0, firstSpace);
		string secondWord;
		if (firstSpace != -1) //if there's a space
			secondWord = command.substr(firstWord.size() + 1, command.size() - 1 - firstWord.size());

		fort::char_table table;
		table.set_border_style(FT_NICE_STYLE); // sets the style for the table
		table << fort::header << "Name" << "ID Number" << "Date of Birth" << "D-dimer Value" << "Sodium Level" << "Potassium Level" << "Priority Value" << fort::endr; //how we print our table

		if (firstWord == "sort") 
		{
			if (secondWord == "id") 
			{
				m.sort(patients, "id");
			}
			else if (secondWord == "name") 
			{
				m.sort(patients, "name");
			}
			else if (secondWord == "bday") 
			{
				m.sort(patients, "bday");
			}
			else if (secondWord == "priority") 
			{
				m.sort(patients, "priority");
			}
			else if (secondWord == "ddimer") 
			{
				m.sort(patients, "ddimer");
			}
			else if (secondWord == "sodium") 
			{
				m.sort(patients, "sodium");
			}
			else if (secondWord == "potassium") 
			{
				m.sort(patients, "potassium");
			}
			else 
			{
				cout << "Invalid sort."; // error, didn't sort anything valid
				continue;
			}
			for (unsigned int i = 0; i < patients.size(); i++) 
			{
				Patient* p = patients[i];
				table << p->patientName << p->patientID << p->patientBday << p->ddimerValue << p->sodiumLevel << p->potassiumLevel << p->priorityValue << fort::endr;
			}
			if (fileOut) {
				file << table.to_string() << endl;
				cout << "File saved." << endl << endl;
			}
			else
				cout << table.to_string() << endl;
		}
		else if (firstWord == "display") 
		{
			size_t found = command.find(" ", firstSpace + 1);
			if (found != string::npos) //we are trying to display a name. An Id doesn't have a space in it
			{
				//Note: the name is stored in the variable secondWord
				vector<Patient*> patientsT;
				m.findPatientNameCall(patientsT, secondWord);
				//the patients vector should have every patient with that name
				for (int i = 0; i < patientsT.size(); i++)
				{
					Patient* p = patientsT[i];
					table << p->patientName << p->patientID << p->patientBday << p->ddimerValue << p->sodiumLevel << p->potassiumLevel << p->priorityValue << fort::endr;
				}
				if (fileOut) {
					file << table.to_string() << endl;
					cout << "File saved." << endl << endl;
				}
				else 
					cout << table.to_string() << endl;
			}
			else 
			{
				//Note: the id is stored in the variable secondWord
				int patientID = stoi(secondWord);
				Patient* p = m[patientID];
				//the patient we want to display should be in p
				table << p->patientName << p->patientID << p->patientBday << p->ddimerValue << p->sodiumLevel << p->potassiumLevel << p->priorityValue << fort::endr;
				if (fileOut) {
					file << table.to_string() << endl;
					cout << "File saved." << endl << endl;
				}
				else
					cout << table.to_string() << endl; 	
			}
		}
		else if (command == "exit") // only way to get out of the while(true)
		{
			break;
		}
	}
	file.close();
	return 0;
}

// https://www.cplusplus.com/reference/random/
// https://www.nrscotland.gov.uk/statistics-and-data/statistics/statistics-by-theme/vital-events/names/babies-first-names/full-lists-of-babies-first-names-2010-to-2014
// https://www.njbcs.net/article.asp?issn=0331-8540;year=2017;volume=14;issue=2;spage=92;epage=95;aulast=Kusfa
