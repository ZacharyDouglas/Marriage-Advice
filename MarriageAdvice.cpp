#include <iostream>
#include <string>
#include <vector>

using std::cout; using std::endl; using std::cin;
using std::string;
using std::vector;

class PersonVisitor;

class Person { // component
public:
	Person(string firstName,
		Person *spouse,
		Person *father,
		Person *mother) :
		firstName_(firstName), spouse_(spouse),
		father_(father), mother_(mother) {}
	string getFirstName() { return firstName_; }
	Person *getSpouse() { return spouse_; }
	void setSpouse(Person *spouse) { spouse_ = spouse; }
	Person *getFather() { return father_; }

	virtual void accept(PersonVisitor *) = 0;
	virtual ~Person() {}
private:
	string firstName_;
	Person *spouse_;
	Person *father_;
	Person *mother_;
};

// man has a last name 
class Man : public Person {
public:
	Man(string lastName,
		string firstName,
		Person *spouse,
		Person *father, Person *mother) :
		lastName_(lastName),
		Person(firstName, spouse, father, mother) {}
	string getLastName() { return lastName_; }
	void accept(PersonVisitor *visitor);
private:
	string lastName_;
};

// woman has a list of children
class Woman : public Person {
public:
	Woman(vector<Person *> children,
		string firstName,
		Person *spouse,
		Person *father, Person *mother) :
		children_(children),
		Person(firstName, spouse, father, mother) {}
	const vector<Person *> &getChildren() { return children_; }
	void setChildren(const vector<Person *> &children) { children_ = children; }
	void accept(PersonVisitor *visitor);
private:
	vector<Person *> children_;
};

// abstract visitor
class PersonVisitor {
public:
	virtual void visit(Man*) = 0;
	virtual void visit(Woman*) = 0;
	virtual ~PersonVisitor() {}
};

// composite methods that define traversal
void Man::accept(PersonVisitor *visitor) { visitor->visit(this); }
void Woman::accept(PersonVisitor *visitor) {
	//children traversal through women only 
	visitor->visit(this);
	for (auto child : children_) // traversing descendants
		child->accept(visitor);
}

// concrete visitors

// the last name for a man is stored in object
// the last name a woman is determined by her 
// spouse if she is married
// or by her father if she is not
class NamePrinter : public PersonVisitor {
public:
	void visit(Man *m) {
		cout << m->getFirstName() << " "
			<< m->getLastName() << endl;
	}
	void visit(Woman *w) {
		cout << w->getFirstName() << " ";
		if (w->getSpouse() != nullptr)
			cout << static_cast<Man *>(w->getSpouse())->getLastName();
		else if (w->getFather() != nullptr)
			cout << static_cast<Man *>(w->getFather())->getLastName();
		else
			cout << "Doe";
		cout << endl;
	}
};

// children for a woman are stored in object
// children for a man are looked up in his spouse's object
class ChildrenPrinter : public PersonVisitor {
public:
	void visit(Man *m) {
		cout << m->getFirstName() << ": ";
		Woman *spouse = static_cast<Woman *>(m->getSpouse());
		if (spouse != nullptr)
			printNames(spouse->getChildren());
		cout << endl;
	}
	void visit(Woman *w) {
		cout << w->getFirstName() << ": ";
		printNames(w->getChildren());
		cout << endl;
	}
private:
	void printNames(const vector<Person *> &children) {
		for (const auto c : children)
			cout << c->getFirstName() << ", ";
	}
};

class MarriageAdvisor : public PersonVisitor {
public:
	MarriageAdvisor() : firstCandidate_(""), secondCandidate_(""), marriageAllowed_(false), currentNodeIsFirstCandidate(false) {};
	MarriageAdvisor(string firstPerson, string secondPerson) : firstCandidate_(firstPerson), secondCandidate_(secondPerson), marriageAllowed_(false), currentNodeIsFirstCandidate(false) {};
	void visit(Man *m);
	void visit(Woman *w);
	//Functions with Signature Type Man
	bool candidatesNameMatchesCurrentNode(Man *m);
	bool candidatesNotCurrentlyMarried(Man *m);
	bool candidatesNotSiblings(Man *m);
	bool candidatesNotAuntOrUncle(Man *m);
	bool candidatesNotNieceOrNephew(Man *m);
	bool candidatesNotCousins(Man *m);
	bool candidatesNotParent(Man *m);
	bool candidatesNotChild(Man *m);

	//Functions with Signature Type Woman
	bool candidatesNameMatchesCurrentNode(Woman *w);
	bool candidatesNotCurrentlyMarried(Woman *w);
	bool candidatesNotSiblings(Woman *w);
	bool candidatesNotAuntOrUncle(Woman *w);
	bool candidatesNotNieceOrNephew(Woman *w);
	bool candidatesNotCousins(Woman *w);
	bool candidatesNotParent(Woman *w);
	bool candidatesNotChild(Woman *w);

	//Generic output of marriage results
	void outputMarriageResult();
private:
	string firstCandidate_;
	string secondCandidate_;
	bool currentNodeIsFirstCandidate;		//bool to track which candidate matches current node name
	bool marriageAllowed_;
};

//Function to output marital approval status for both man and woman
void MarriageAdvisor::outputMarriageResult() {
	if (marriageAllowed_ == true)
		cout << "They can marry!" << endl;
	else
		cout << "They cannot marry!" << endl;
	exit(0);
}

void MarriageAdvisor::visit(Man *m) {
	const string fullName = m->getFirstName() + " " + m->getLastName();
	if (m->getFather() != nullptr) {
		if (candidatesNameMatchesCurrentNode(m)) {
			if (candidatesNotCurrentlyMarried(m)) {
				if (candidatesNotSiblings(m)) {
					if (candidatesNotAuntOrUncle(m)) {
						if (candidatesNotNieceOrNephew(m)) {
							if (candidatesNotCousins(m)) {
								if (candidatesNotParent(m)) {
									const auto &dadPtr = m->getFather();
									Woman *momPtr = static_cast<Woman *>(dadPtr->getSpouse());
									const auto &childrenVec = momPtr->getChildren();
									if (childrenVec.empty() == false) {
										if (candidatesNotChild(m)) {
											marriageAllowed_ = true;
											outputMarriageResult();
										}
									}
									else {
										marriageAllowed_ = true;
										outputMarriageResult();
									}
								}
							}
						}
					}
				}
			}
		}
	}
}


void MarriageAdvisor::visit(Woman *w) {
	if (candidatesNameMatchesCurrentNode(w)) {
		if (candidatesNotCurrentlyMarried(w)) {
			if (candidatesNotSiblings(w)) {
				if (candidatesNotAuntOrUncle(w)) {
					if (candidatesNotNieceOrNephew(w)) {
						if (candidatesNotCousins(w)) {
							if (candidatesNotParent(w)) {
								if (candidatesNotChild(w)) {
									marriageAllowed_ = true;
									outputMarriageResult();
								}
							}
						}
					}
				}
			}
		}
	}
}

//Woman * Function to check if current node matches either candidate name
bool MarriageAdvisor::candidatesNameMatchesCurrentNode(Woman *w) {
	//Try to fetch last name via father of woman
	//If no father, try to fetch last name via spouse of woman
	string lastName;
	if (w->getFather()) {
		Man *dadPtr = static_cast<Man *>(w->getFather());
		lastName = dadPtr->getLastName();
	}
	else {
		if (w->getSpouse()) {
			Man *spousePtr = static_cast<Man *>(w->getSpouse());
			lastName = spousePtr->getLastName();
		}
		else
			return false;
	}
	string fullName = w->getFirstName() + " " + lastName;
	if (fullName == firstCandidate_) {
		currentNodeIsFirstCandidate = true;
		return true;
	}
	else {
		if (fullName == secondCandidate_) {
			currentNodeIsFirstCandidate = false;
			return true;
		}
		else {
			return false;
		}
	}
}

//Woman * Function to check if candidate is already married
bool MarriageAdvisor::candidatesNotCurrentlyMarried(Woman *w) {
	if (w->getSpouse() != nullptr) {
		outputMarriageResult();
		return false;
	}else
		return true;
}

//Woman * Function to check if candidates are siblings
bool MarriageAdvisor::candidatesNotSiblings(Woman *w) {
	Man *fatherPtr = static_cast<Man *>(w->getFather());
	Woman *motherPtr = static_cast<Woman *>(fatherPtr->getSpouse());
	const auto &siblingsVec = motherPtr->getChildren();
	for (const auto &sibling : siblingsVec) {
		const string siblingName = sibling->getFirstName() + " " + fatherPtr->getLastName();
		if (currentNodeIsFirstCandidate == true) {
			if (secondCandidate_ == siblingName)
				outputMarriageResult();
		}
		else {
			if (firstCandidate_ == siblingName)
				outputMarriageResult();
		}
	}
	return true;
}

//Woman * Function to check if candidates are Aunt/Uncle
bool MarriageAdvisor::candidatesNotAuntOrUncle(Woman *w) {
	Man *fatherPtr = static_cast<Man *>(w->getFather());
	const auto &motherPtr = fatherPtr->getSpouse();
	Man *dadsFather = static_cast<Man *>(fatherPtr->getFather());
	Man *momsFather = static_cast<Man *>(motherPtr->getFather());
	Woman *dadsMother = static_cast<Woman *>(dadsFather->getSpouse());
	Woman *momsMother = static_cast<Woman *>(momsFather->getSpouse());
	const auto &momsSideVec = momsMother->getChildren();
	const auto &dadsSideVec = dadsMother->getChildren();
	
	//Check mom's brothers and sisters 
	for (const auto &auntOrUncle : momsSideVec) {
		Man *dadPtr = static_cast<Man *>(auntOrUncle->getFather());
		const string auntOrUncleName = auntOrUncle->getFirstName() + " " + dadPtr->getLastName();
		if (currentNodeIsFirstCandidate == true) {
			if (secondCandidate_ == auntOrUncleName)
				outputMarriageResult();
		}else{
			if (firstCandidate_ == auntOrUncleName)
				outputMarriageResult();
		}
	}

	//Check dad's brothers and sisters
	for (const auto &auntOrUncle : dadsSideVec) {
		Man *dadPtr = static_cast<Man *>(auntOrUncle->getFather());
		const string auntOrUncleName = auntOrUncle->getFirstName() + " " + dadPtr->getLastName();
		if (currentNodeIsFirstCandidate == true) {
			if (secondCandidate_ == auntOrUncleName)
				outputMarriageResult();
		}
		else {
			if (firstCandidate_ == auntOrUncleName)
				outputMarriageResult();
		}
	}
	return true;
}

//Woman * Function to check if candidates are nieces/nephews
bool MarriageAdvisor::candidatesNotNieceOrNephew(Woman *w) {
	Man *fatherPtr = static_cast<Man *>(w->getFather());
	Woman *motherPtr = static_cast<Woman *>(fatherPtr->getSpouse());
	const auto &siblingsVec = motherPtr->getChildren();

	for (const auto &sibling : siblingsVec) {
		Woman *siblingPtr = static_cast<Woman *>(sibling);
		const auto &niecesAndNephews = motherPtr->getChildren();
		for (const auto &nieceOrNephew : niecesAndNephews) {
			Man *fatherPtr = static_cast<Man *>(nieceOrNephew->getFather());
			string nieceOrNephewName = nieceOrNephew->getFirstName() + " " + fatherPtr->getLastName();
			if (currentNodeIsFirstCandidate == true) {
				if (secondCandidate_ == nieceOrNephewName)
					outputMarriageResult();
			}
			else {
				if (firstCandidate_ == nieceOrNephewName)
					outputMarriageResult();
			}
		}
	}
	return true;
}

//Woman * Function to check if candidates are cousins
bool MarriageAdvisor::candidatesNotCousins(Woman *w) {
	Man *fatherPtr = static_cast<Man *>(w->getFather());
	const auto &motherPtr = fatherPtr->getSpouse();
	Man *dadsFather = static_cast<Man *>(fatherPtr->getFather());
	Man *momsFather = static_cast<Man *>(motherPtr->getFather());
	Woman *dadsMother = static_cast<Woman *>(dadsFather->getSpouse());
	Woman *momsMother = static_cast<Woman *>(momsFather->getSpouse());
	const auto &momsSideVec = momsMother->getChildren();
	const auto &dadsSideVec = dadsMother->getChildren();

	//Check mom's side of the family
	//Nested For-range loops to iterate through each aunt/uncle
	//And then iterate through all the cousins of each aunt/uncle
	for (const auto &auntOrUncle : momsSideVec) {
		Woman *auntOrUnclePtr = static_cast<Woman *>(auntOrUncle);
		const auto &cousinsVec = auntOrUnclePtr->getChildren();
		for (const auto &cousin : cousinsVec) {
			Man *dadPtr = static_cast<Man *>(cousin->getFather());
			const string cousinName = cousin->getFirstName() + " " + dadPtr->getLastName();
			if (currentNodeIsFirstCandidate == true) {
				if (secondCandidate_ == cousinName)
					outputMarriageResult();
			}else{
				if (firstCandidate_ == cousinName)
					outputMarriageResult();
			}
		}
	}

	//Check dad's side of the family
	//Nested For-range loops to iterate through each aunt/uncle
	//And then iterate through all the cousins of each aunt/uncle
	for (const auto &auntOrUncle : dadsSideVec) {
		Woman *auntOrUnclePtr = static_cast<Woman *>(auntOrUncle);
		const auto &cousinsVec = auntOrUnclePtr->getChildren();
		for (const auto &cousin : cousinsVec) {
			Man *dadPtr = static_cast<Man *>(cousin->getFather());
			const string cousinName = cousin->getFirstName() + " " + dadPtr->getLastName();
			if (currentNodeIsFirstCandidate == true) {
				if (secondCandidate_ == cousinName)
					outputMarriageResult();
			}
			else {
				if (firstCandidate_ == cousinName)
					outputMarriageResult();
			}
		}
	}
	return true;
}

//Woman * Function to check if candidates are parents
bool MarriageAdvisor::candidatesNotParent(Woman *w) {
	Man *fatherPtr = static_cast<Man *>(w->getFather());
	Woman *motherPtr = static_cast<Woman *>(w->getFather());
	const string motherName = motherPtr->getFirstName() + " " + fatherPtr->getLastName();
	const string fatherName = fatherPtr->getFirstName() + " " + fatherPtr->getLastName();
	if (currentNodeIsFirstCandidate == true) {
		if (secondCandidate_ == motherName || secondCandidate_ == fatherName)
			outputMarriageResult();
		else {
			if (firstCandidate_ == motherName || secondCandidate_ == fatherName)
				outputMarriageResult();
		}
	}

	const auto &childrenVec = w->getChildren();
	for (const auto &child : childrenVec) {
		Man *dadPtr = static_cast<Man *>(child->getFather());
		const string childName = child->getFirstName() + " " + dadPtr->getLastName();
		if (currentNodeIsFirstCandidate == true) {
			if (secondCandidate_ == childName)
				outputMarriageResult();
		}
		else {
			if (firstCandidate_ == childName)
				outputMarriageResult();
		}
	}
	return false;
}

//Woman * Function to check if candidates are children
bool MarriageAdvisor::candidatesNotChild(Woman *w) {
	const auto &childrenVec = w->getChildren();

	for (const auto &child : childrenVec) {
		Man *dadPtr = static_cast<Man *>(child);
		const string childName = child->getFirstName() + " " + dadPtr->getLastName();
		if (currentNodeIsFirstCandidate == true) {
			if (secondCandidate_ == childName)
				outputMarriageResult();
		}else {
			if (firstCandidate_ == childName)
				outputMarriageResult();
		}
	}
	return true;
}

//Man * Function to check if current node matches either candidate name
bool MarriageAdvisor::candidatesNameMatchesCurrentNode(Man *m) {
	string name = m->getFirstName() + " " + m->getLastName();
	if (name == firstCandidate_) {
		//cout << "Current Node Matches First Candidate!" << endl;
		//cout << "\tMatching Node Name:  " << m->getFirstName() + " " + m->getLastName() << endl << "\tFirst Candidate Matching Name:  " << firstCandidate_ << endl;
		currentNodeIsFirstCandidate = true;
		return true;
	}else {
		if (name == secondCandidate_) {
			//cout << "Current Node Matches Second Candidate!" << endl;
			//cout << "\tMatching Node Name:  " << m->getFirstName() + " " + m->getLastName() << endl << "\tSecond Candidate Matching Name:  " << secondCandidate_ << endl;
			currentNodeIsFirstCandidate = false;
			return true;
		}
		else
			return false;
	}
}

//Man * Function to check if candidate is married
bool MarriageAdvisor::candidatesNotCurrentlyMarried(Man *m) {
	const auto &spousePtr = m->getSpouse();
	if (spousePtr == nullptr) {
		//cout << m->getFirstName() + " " + m->getLastName() << " is NOT married!" << endl;
		return true;
	}
	else {
		//cout << m->getFirstName() + " " + m->getLastName() << " IS married!" << endl;
		//cout << "\tSpouse Name:  " << spousePtr->getFirstName() + " " + m->getLastName() << endl;
		outputMarriageResult();
		return false;
	}
}

//Man * Function to check if candidates are siblings
bool MarriageAdvisor::candidatesNotSiblings(Man *m) {
	const auto &fatherPtr = m->getFather();	
	Woman *motherPtr = static_cast<Woman *>(fatherPtr->getSpouse());
	const auto &childrenVec = motherPtr->getChildren();

	//For each child in childrenVec, get child's full name
	//See if child name matches candidate name
	//If not, return true.  Else, return false
	for (const auto & child : childrenVec) {
			const string childName = child->getFirstName() + " " + m->getLastName();
			//cout << "Checking child " << childName << endl;
			if (currentNodeIsFirstCandidate == true) {
				if (secondCandidate_ == childName) {
					outputMarriageResult();
				}
			}
			else {
				if (firstCandidate_ == childName) {
					outputMarriageResult();
				}
			}
	}
	return true;
}

//Man * Function to check if candidates are Aunt/Uncle
bool MarriageAdvisor::candidatesNotAuntOrUncle(Man *m) {
	const auto &fatherPtr = m->getFather();
	Woman *motherPtr = static_cast<Woman *>(fatherPtr->getSpouse());
	if (fatherPtr->getFather() != nullptr) {
		const auto &dadsFather = fatherPtr->getFather();
		const auto &momsFather = motherPtr->getFather();
		if (momsFather->getSpouse() != nullptr) {
			Woman *momsMother = static_cast<Woman *>(momsFather->getSpouse());
			const auto &momsSideVec = momsMother->getChildren();
			//Check mom's brothers and sisters
			if (momsSideVec.empty() == false) {
				for (const auto &auntOrUncle : momsSideVec) {
					Man *dadPtr = static_cast<Man *>(auntOrUncle->getFather());
					const string auntOrUncleName = auntOrUncle->getFirstName() + " " + dadPtr->getLastName();
					if (currentNodeIsFirstCandidate == true) {
						if (secondCandidate_ == auntOrUncleName) {
							outputMarriageResult();
						}
					}
					else {
						if (firstCandidate_ == auntOrUncleName) {
							outputMarriageResult();
						}
					}
				}
			}
		}
		if (momsFather->getSpouse() != nullptr) {
			Woman *dadsMother = static_cast<Woman *>(dadsFather->getSpouse());
			const auto &dadsSideVec = dadsMother->getChildren();
			//Check dad's brothers and sisters
			if (dadsSideVec.empty() == true) {
				for (const auto &auntOrUncle : dadsSideVec) {
					Man *dadPtr = static_cast<Man *>(auntOrUncle->getFather());
					const string auntOrUncleName = auntOrUncle->getFirstName() + " " + dadPtr->getLastName();
					if (currentNodeIsFirstCandidate == true) {
						if (secondCandidate_ == auntOrUncleName) {
							outputMarriageResult();
						}
					}
					else {
						if (firstCandidate_ == auntOrUncleName)
							outputMarriageResult();
					}
				}
			}
		}
	}
	else {
		outputMarriageResult();
	}

	return true;
}

//Man * Function to check if candidates are nieces/nephews
bool MarriageAdvisor::candidatesNotNieceOrNephew(Man *m) {
	const auto &fatherPtr = m->getFather();
	Woman *motherPtr = static_cast<Woman *>(fatherPtr->getSpouse());
	const auto &siblingsVec = motherPtr->getChildren();

	for (const auto &sibling : siblingsVec) {
		Woman *siblingPtr = static_cast<Woman *>(sibling);
		const auto &niecesAndNephewsVec = siblingPtr->getChildren();
		for (const auto &nieceOrNephew : niecesAndNephewsVec) {
			Man *fatherPtr = static_cast<Man *>(nieceOrNephew->getFather());
			string name = nieceOrNephew->getFirstName() + " " + fatherPtr->getLastName();
			if (currentNodeIsFirstCandidate == true) {
				if (secondCandidate_ == name)
					outputMarriageResult();
			}
			else {
				if (firstCandidate_ == name)
					outputMarriageResult();
			}
		}
	}
	return true;
}

//Man * Function to check if candidates are cousins
bool MarriageAdvisor::candidatesNotCousins(Man *m) {
	const auto &fatherPtr = m->getFather();
	Woman *motherPtr = static_cast<Woman *>(fatherPtr->getSpouse());
	const auto &dadsFather = fatherPtr->getFather();
	Woman *momsFather = static_cast<Woman *>(motherPtr->getFather());
	Woman *dadsMother = static_cast<Woman *>(dadsFather->getSpouse());
	Woman *momsMother = static_cast<Woman *>(momsFather->getSpouse());
	const auto &momsSideVec = momsMother->getChildren();
	const auto &dadsSideVec = dadsMother->getChildren();

	//Check mom's side of the family
	//Nested For-range loops to iterate through each aunt/uncle
	//And then iterate through all the cousins of each aunt/uncle
	for (const auto &auntOrUncle : momsSideVec) {
		Woman *auntOrUnclePtr = static_cast<Woman *>(auntOrUncle);
		const auto& cousinsVec = auntOrUnclePtr->getChildren();
		for (const auto &cousin : cousinsVec) {
			Man *fatherPtr = static_cast<Man *>(cousin->getFather());
			string cousinName = cousin->getFirstName() + " " + fatherPtr->getLastName();
			if (currentNodeIsFirstCandidate == true) {
				if (secondCandidate_ == cousinName)
					outputMarriageResult();
			}else {
				if (firstCandidate_ == cousinName)
					outputMarriageResult();
			}
		}	
	}

	//Check dad's side of the family
	//Nested For-range loops to iterate through each aunt/uncle
	//And then iterate through all the cousins of each aunt/uncle
	for (const auto &auntOrUncle : dadsSideVec) {
		Woman *auntOrUnclePtr = static_cast<Woman *>(auntOrUncle);
		const auto& cousinsVec = auntOrUnclePtr->getChildren();
		for (const auto &cousin : cousinsVec) {
			Man *fatherPtr = static_cast<Man *>(cousin->getFather());
			string cousinName = cousin->getFirstName() + " " + fatherPtr->getLastName();
			if (currentNodeIsFirstCandidate == true) {
				if (secondCandidate_ == cousinName)
					outputMarriageResult();
			}
			else {
				if (firstCandidate_ == cousinName)
					outputMarriageResult();
			}
		}
	}
	return true;
}

//Man * Function to check if candidates are parents
bool MarriageAdvisor::candidatesNotParent(Man *m) {
	//First, check if candidates are Parents
	const auto &fatherPtr = m->getFather();
	Woman *motherPtr = static_cast<Woman *>(fatherPtr->getSpouse());
	Man *father = static_cast<Man *>(m->getFather());
	const string motherName = motherPtr->getFirstName() + " " + father->getLastName();
	const string fatherName = fatherPtr->getFirstName() + " " + father->getLastName();
	if (currentNodeIsFirstCandidate == true) {
		if ((secondCandidate_ == motherName) || (secondCandidate_ == fatherName))
			outputMarriageResult();
	}else {
		if ((firstCandidate_ == motherName) || (firstCandidate_ == fatherName))
			outputMarriageResult();
	}

	//Second, check that candidates are not your children
	Woman *mom = static_cast<Woman *>(m->getSpouse());
	const auto &childrenVec = mom->getChildren();
	for (const auto &child : childrenVec) {
		Man *dadPtr = static_cast<Man *>(child->getFather());
		const string childName = child->getFirstName() + " " + dadPtr->getLastName();
		if (currentNodeIsFirstCandidate == true) {
			if (firstCandidate_ == childName)
				outputMarriageResult();
		}else {
			if (firstCandidate_ == childName)
				outputMarriageResult();
		}
	}
	return true;
}

//Man * Function to check if candidates are children
bool MarriageAdvisor::candidatesNotChild(Man *m) {
	Woman *spousePtr = static_cast<Woman *>(m->getSpouse());
	const auto &childrenVec = spousePtr->getChildren();

	for (const auto &child : childrenVec) {
		Man *dadPtr = static_cast<Man *>(child);
		const string childName = child->getFirstName() + " " + dadPtr->getLastName();
		if (currentNodeIsFirstCandidate == true) {
			if (secondCandidate_ == childName)
				outputMarriageResult();
		}
		else {
			if (firstCandidate_ == childName)
				outputMarriageResult();
		}
	}
	return true;
}

// demonstrating the operation
int main() {

	// setting up the genealogical tree

	// first generation
	Man *js = new Man("Smith", "James", nullptr, nullptr, nullptr);
	Woman *ms = new Woman({}, "Mary", nullptr, nullptr, nullptr);
	ms->setSpouse(js); js->setSpouse(ms);

	// second generation
	Woman *ps = new Woman({}, "Patricia", nullptr, js, ms);
	Man *wj = new Man("Johnson", "William", nullptr, nullptr, nullptr);
	ps->setSpouse(wj); wj->setSpouse(ps);

	vector<Person *> marysKids = { ps,
		new Man("Smith", "Robert", nullptr, js, ms),
		new Woman({}, "Linda", nullptr, js, ms) };
	ms->setChildren(marysKids);

	// third generation
	Man *mj = new Man("Johnson", "Michael", nullptr, wj, ps);
	vector<Person *> patsKids = { mj,
		new Woman({}, "Barbara", nullptr, wj, ps) };
	ps->setChildren(patsKids);

	Woman *jj = new Woman({}, "Jennifer", nullptr, nullptr, nullptr);
	vector<Person *> jensKids = { new Woman({}, "Susan", nullptr, mj ,jj) };

	jj->setSpouse(mj); mj->setSpouse(jj);
	jj->setChildren(jensKids);


	// defining two visitors
	ChildrenPrinter *cp = new ChildrenPrinter;
	NamePrinter *np = new NamePrinter;

	string marriageCandidateOne; string marriageCandidateTwo;

	cout << "Enter one marriage candidate:  ";
	std::getline(cin, marriageCandidateOne);

	cout << "Enter another marriage candidate:  ";
	std::getline(cin, marriageCandidateTwo);

	MarriageAdvisor *ma = new MarriageAdvisor(marriageCandidateOne, marriageCandidateTwo);

	ms->accept(ma);
}

