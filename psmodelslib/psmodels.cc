
#include "smodels.h"
#include "api.h"
#include "read.h"
#include "atomrule.h"
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <vector>
#include <list>
#include <sstream>
#include <iostream>
#include "psmodels.h"

typedef vector<bool> Model;

typedef list<vector<bool>*> ModelList;

#define BUFSIZE_LOCAL 1024

struct Connection
{
	Connection() : generator_sat(0), tester_old_sat(0),
			tester_sat(0), choice(0), degree(0) { }
	Atom *generator_sat;
	Atom *tester_old_sat;
	Atom *tester_sat;
	size_t choice;
	size_t degree;
};

struct MetaPriority
{
	MetaPriority() : old_meta_atom(0), new_meta_atom(0),
			more_important(0), less_important(0),
			selected(0) { }
	Atom *old_meta_atom;
	Atom *new_meta_atom;

	size_t more_important;
	size_t less_important;
	bool selected;
};

class Tester : public Smodels
{
public:
	Tester() : Smodels(), use_interactive_priorities(0), number_of_choices(0),
	number_of_alternatives(0), model_calls(0), choice(0),
	sat_to_old_sat(0), meta(0), max_choice_length(0) { }

	void set_compute(); //Api &tester, Smodels &generator);
	void print_best_model(bool);
	void store_model(Smodels &s);
	bool use_interactive_priorities;
	size_t number_of_choices;
	size_t number_of_alternatives;

	size_t model_calls;

	char **choice;
	Connection *sat_to_old_sat;
	long **choice_to_atom;
	MetaPriority *meta;

	ModelList found_models;
	size_t max_choice_length;
};

void Tester::print_best_model(bool print_meta = false)
{
	ModelList::iterator mod = found_models.begin();
	Model m = **mod;
	cout << "Stable Model: ";
	for (Node *n = program.atoms.head (); n; n = n->next) {
		if (((m[n->atom->index] == true) && n->atom->name)) {
			if (print_meta || (*n->atom->name != '*'))
				cout << n->atom->name << ' ';
		}
	}
	cout << endl;
}


void set_compute(Tester &t)
{
	t.revert();

	// set values for *old_sat atoms
	for (size_t i = 0; i < t.number_of_alternatives; i++) {
		if (t.sat_to_old_sat[i].generator_sat->Bpos) {
			t.sat_to_old_sat[i].tester_old_sat->computeTrue = true;
			t.sat_to_old_sat[i].tester_old_sat->computeFalse = false;

		} else {
			t.sat_to_old_sat[i].tester_old_sat->computeFalse = true;
			t.sat_to_old_sat[i].tester_old_sat->computeTrue = false;

		}
	}

	if (t.use_interactive_priorities) {
		size_t num_combinations =
				t.number_of_choices * (t.number_of_choices-1);
		for (size_t i = 0; i < num_combinations; i++) {
			if (t.meta[i].selected) {
				t.meta[i].new_meta_atom->computeTrue = true;
				t.meta[i].new_meta_atom->computeFalse = false;
			} else {
				t.meta[i].new_meta_atom->computeTrue = false;
				t.meta[i].new_meta_atom->computeFalse = true;
			}
		}
	}

	return;
}

void Tester::store_model(Smodels &g)
{
	Model *m = new Model(g.program.number_of_atoms);

	for (Node *n = g.program.atoms.head (); n; n = n->next) {
		if (n->atom->Bpos) {
			(*m)[n->atom->index] = true;
		} else {
			(*m)[n->atom->index] = false;
		}
	}
	found_models.push_front(m);
}

void update_tester(Tester &t)
{
	// first, save the current model
	Model *m = new Model(t.program.number_of_atoms);

	for (Node *n = t.program.atoms.head (); n; n = n->next) {
		if (n->atom->Bpos) {
			(*m)[n->atom->index] = true;
		} else {
			(*m)[n->atom->index] = false;
		}
	}

	t.found_models.push_front(m);

	t.revert();

	for (size_t i = 0; i < t.number_of_alternatives; i++) {
		if ((*m)[t.sat_to_old_sat[i].tester_sat->index]) {
			t.sat_to_old_sat[i].tester_old_sat->computeTrue = true;
			t.sat_to_old_sat[i].tester_old_sat->computeFalse = false;
		} else {
			t.sat_to_old_sat[i].tester_old_sat->computeTrue = false;
			t.sat_to_old_sat[i].tester_old_sat->computeFalse = true;
		}
	}
}

void print_truth_values(Smodels &t)
{
	for (long i = 0; i < t.program.number_of_atoms; i++) {
		if (t.atom[i]->name) {
			cerr << i << " " << t.atom[i]->name << ": " << t.atom[i]->Bpos << " "
					<< t.atom[i]->Bneg << endl;
		}
	}
}

void Tester::set_compute()//Api &tester, Smodels &generator)
{
	//    revert();

	//    // set values for *old_sat atoms
	//    for (size_t i = 0; i < number_of_alternatives; i++) {
	//      if (sat_to_old_sat[i].first->Bpos) {
	//        sat_to_old_sat[i].second->computeTrue = true;
	//      } else {
	//        sat_to_old_sat[i].second->computeFalse = true;
	//      }
	//    }

	//    if (use_interactive_priorities) {
	//      size_t num_combinations =
	//        number_of_choices * (number_of_choices-1);
	//      for (size_t i = 0; i < num_combinations; i++) {
	//        if (meta[i].old_meta_atom->Bpos) {
	//  	meta[i].new_meta_atom->computeTrue = true;
	//        } else {
	//  	meta[i].new_meta_atom->computeFalse = true;
	//        }
	//      }
	//    }

}

class TesterReader : public Read
{
public:
	TesterReader (Program *p, Api *a) :
		Read(p, a) { }
	bool read_tester_rules(istream &f);
};

bool TesterReader::read_tester_rules(istream &f)
{
	int type;
	bool stop = false;
	for ( ; !stop; linenumber++) {
		// Rule Type
		f >> type;
		switch (type) {
		case ENDRULE:
			stop = true;
			break;
		case BASICRULE:
			if (addBasicRule (f))
				return 1;
			break;
		case CONSTRAINTRULE:
			if (addConstraintRule (f))
				return 1;
			break;
		case CHOICERULE:
			if (addChoiceRule (f))
				return 1;
			break;
		case GENERATERULE:
			if (addGenerateRule (f))
				return 1;
			break;
		case WEIGHTRULE:
			if (addWeightRule (f))
				return 1;
			break;
		case OPTIMIZERULE:
			if (addOptimizeRule (f))
				return 1;
			break;
		default:
			return 1;
		}
	}
	return true;
}

bool read_generator(Api &generator, Smodels &sm, ifstream &in)
{
	generator.remember();

	Read reader(&sm.program, &generator);

	if (reader.read(in))
		return false; // error
	generator.done();

	return true;
}

bool read_tester(Api &tester, Api &generator, Tester &t, Smodels &g,
		ifstream &in)
{
	tester.remember();
	//  tester.copy(&generator);

	TesterReader read(&t.program, &tester);

	if (read.read(in))
		return false; //error

	if (!read.read_tester_rules(in)) {
		return false;
	}

	tester.done();
	g.init();
	t.init();

	// read additional data
	char buf[BUFSIZE_LOCAL] = { 0 };

	// flush the line
	in.getline(buf, BUFSIZE_LOCAL);

	// number of choices
	in.getline(buf, BUFSIZE_LOCAL);
	t.number_of_choices = strtol(buf, 0, 10);
	t.choice_to_atom = new long*[t.number_of_choices+1];
	// number of alternatives
	in.getline(buf, BUFSIZE_LOCAL);
	t.number_of_alternatives = strtol(buf, 0, 10);

	// meta-priorities
	in.getline(buf, BUFSIZE_LOCAL);
	t.use_interactive_priorities = strtol(buf, 0, 10);

	// rules that correspond to choices:
	t.choice = new char*[t.number_of_choices];
	char *p;
	long no, degree;
	for (size_t i = 0; i < t.number_of_choices; i++) {
		in >> no;
		in >> degree;
		in.getline(buf, BUFSIZE_LOCAL);
		p = buf;
		while (!isspace(*p)) p++;
		p++;
		t.choice[i] = strdup(p);
		if (strlen(p) > t.max_choice_length) {
			t.max_choice_length = strlen(p);
		}
		t.choice_to_atom[i+1] = new long[degree + 1];
	}
	// read separator
	in.getline(buf, BUFSIZE_LOCAL);

	// read correspondence between *sat and *old_sat
	t.sat_to_old_sat = new Connection[t.number_of_alternatives];

	Atom *sat, *old_sat;
	char *f, *s;

	for (size_t i = 0; i < t.number_of_alternatives; i++) {
		long choice, degree;
		in.getline(buf, BUFSIZE_LOCAL);
		s = f = buf;
		while (!isspace(*s)) s++;
		*s = '\0';
		s++;

		sat = generator.get_atom(f);
		old_sat = tester.get_atom(s);

		t.sat_to_old_sat[i].generator_sat = sat;
		t.sat_to_old_sat[i].tester_old_sat = old_sat;
		sat = tester.get_atom(f);
		t.sat_to_old_sat[i].tester_sat = sat;
		while (*s != '(') s++;
		s++;
		choice = strtol(s, &s, 10);
		s++;
		degree = strtol(s, 0, 10);
		t.choice_to_atom[choice][degree]=i;
		t.sat_to_old_sat[i].choice = choice;
		t.sat_to_old_sat[i].degree = degree;
	}
	// flush the line
	//  in.getline(buf, BUFSIZE_LOCAL);
	// read the separator
	in.getline(buf, BUFSIZE_LOCAL);

	if (t.use_interactive_priorities) {
		size_t more_important, less_important;
		// read priority connections
		size_t num_combinations =
				t.number_of_choices *(t.number_of_choices-1);
		t.meta = new MetaPriority[num_combinations];

		for (size_t i = 0; i < num_combinations; i++) {
			in.getline(buf, BUFSIZE_LOCAL);
			char *p = buf;
			while (!isspace(*p)) p++;
			*p = '\0';
			p++;
			more_important = strtol(p, &p, 10);
			less_important = strtol(p, &p, 10);

			//      t.meta[i].old_meta_atom = generator.get_atom(buf);
			t.meta[i].new_meta_atom = tester.get_atom(buf);
			t.meta[i].more_important = more_important;
			t.meta[i].less_important = less_important;
		}
		// flush the line
		//    in.getline(buf, BUFSIZE_LOCAL);
	}

	return true;
}

void print_model(Model *m, Smodels &g, bool print_meta = false)
{
	for (Node *n = g.program.atoms.head (); n; n = n->next) {
		if ((((*m)[n->atom->index] == true) && n->atom->name)) {
			if (print_meta || (*n->atom->name != '*'))
				cout << n->atom->name << ' ';
		}
	}
}

void define_meta_priority(Tester &t, char *p)
{
	size_t more_important;
	size_t less_important;

	more_important = strtol(p, &p, 10);
	p++;
	less_important = strtol(p, &p, 10);

	size_t limit = t.number_of_choices * (t.number_of_choices-1);
	for (size_t i = 0; i < limit; i++) {
		if ((t.meta[i].more_important == more_important) &&
				(t.meta[i].less_important == less_important)) {
			t.meta[i].selected = true;
			break;
		}
	}
}

void print_satisfactions(Model *m, Smodels &g, Tester &t)
{
	static int *degree;

	if (!degree) {
		degree = new int[t.number_of_choices+1];
	}
	for (size_t i = 0; i < t.number_of_alternatives; i++) {
		if ((*m)[t.sat_to_old_sat[i].generator_sat->index]) {
			degree[t.sat_to_old_sat[i].choice-1] = t.sat_to_old_sat[i].degree;
		}
	}
	for (size_t i = 0 ; i  < t.number_of_choices; i++) {
		cout << "\t" << t.choice[i] << "\t sat to degree " << degree[i] << endl;
	}
}

ostream &padded(ostream &os, char *c, size_t len)
{
	size_t i;

	i = len - strlen(c);

	os << c;

	if (i > 0) {
		for (size_t j = 0; j < i; j++) {
			os << ' ';
		}
	}

	return os;
}

ostream &padded(ostream &os, size_t num, size_t len)
{
	size_t i, pos;

	//  i = uf);

	os << num << "FIXME";

	//  if (i > 0) {
	//    for (size_t j = 0; j < i; j++) {
	//      os << ' ';
	//    }
	//  }

	return os;
}

void explain_differences(Model *m1, Model *m2,
		size_t i1, size_t i2,
		Tester &t)
{
	static int *d1, *d2;

	if (!d1) {
		d1 = new int[t.number_of_choices];
		d2 = new int[t.number_of_choices];
	}
	for (size_t i = 0; i < t.number_of_alternatives; i++) {
		if ((*m1)[t.sat_to_old_sat[i].generator_sat->index]) {
			d1[t.sat_to_old_sat[i].choice-1] = t.sat_to_old_sat[i].degree;
		}
		if ((*m2)[t.sat_to_old_sat[i].generator_sat->index]) {
			d2[t.sat_to_old_sat[i].choice-1] = t.sat_to_old_sat[i].degree;
		}
	}

	padded(cout, "Choice", t.max_choice_length + 3);
	padded(cout, i1, 5);
	padded(cout, i2, 5);
	cout << endl;

	for (size_t i = 0; i < t.max_choice_length + 13; i++) {
		cout << '-';
	}
	cout << endl;
	for (size_t i = 0; i < t.number_of_choices; i++) {
		if (d1[i] != d2[i]) {
			padded(cout, t.choice[i], t.max_choice_length+3);
			padded(cout, d1[i], 5);
			padded(cout, d2[i], 5);
			cout << endl;
		}
	}
}



void uncomparable_models(Smodels &g, Tester &t)
{
	static char buf[BUFSIZE_LOCAL];
	cout << "Uncomparable models: " << t.found_models.size() << endl;
	ModelList::iterator p = t.found_models.begin();

	size_t n = 0;
	for ( ; p != t.found_models.end(); p++) {
		Model *m = *p;
		cout << ++n << ": { " ;
		print_model(m, g);
		cout << "} " << endl;

		//    print_satisfactions(m, g, t);
	}

	//  resolve_differences();
	Model *m1 = *t.found_models.begin();
	Model *m2 = *(++t.found_models.begin());

	explain_differences(m1, m2, 1, 2, t);
	//  cout << "New priorities? " << flush;

	//    cin.getline(buf, BUFSIZE_LOCAL);
	//    char *st = buf;
	//    while (*st && isspace(*st)) {
	//      st++;
	//    }
	//    if (isdigit(*st)) {
	//      define_meta_priority(t, st);

	//    }
	return ;
}

void interactive_models(Smodels &g, Tester &t)
{
	bool end = false, found;

	while (!end) {
		found = g.model();

		//    g.printAnswer();
		if (!found) {
			end = true;
		} else {
			//      cout << "Generator model: ";
			//      g.printAnswer();

			set_compute(t);

			found = t.model();
			t.model_calls++;
			if (!found) {
				t.store_model(g);
				//	cout << "No better found" << endl;
				//  	cout << "Answer: " << ++models_found << endl;
				//  	g.printAnswer();
			} else {
				//	cout << "Tester model: ";
				//	t.printAnswer();
			}
		}
	}

	if (t.found_models.size() == 0) {
		cout << "No models" << endl;
	} else if (t.found_models.size() == 1) {
		cout << "Unique model: " ;
		print_model(*t.found_models.begin(), g);
		cout << endl;
	} else {
		uncomparable_models(g, t);
	}
}



//int main(int argc, char *argv[])
int psmodels(int argc, char *argv[],char *output_file)
{
	Smodels g;
	Tester t;
	Api generator(&g.program);
	Api tester(&t.program);
	char *fname = 0;
	size_t models = 1;

	for (int i = 1; i < argc; i++) {
		if (isdigit(*argv[i])) {
			models = strtol(argv[i], 0, 10);
		} else {
			fname = argv[i];
		}
	}
	if (!fname) {
		cerr << "usage: psmodels [models] input_file" << endl;
		exit(1);
	}
	ifstream in(fname);

	if (!read_generator(generator, g, in)) {
		cerr << "error in input (generator)" << endl;
	}
	in.close();
	//cout << "prova " << endl;
	//g.printStack();



	ifstream in2(fname);
	if (!read_tester(tester, generator, t, g, in2)) {
		cerr << "error in input (tester)" << endl;
	}

	in2.close();




	bool end = false;
	bool found = false;
	size_t models_found = 0;

	FILE* stream;
	fpos_t pos;
	int    fd;
	//redirectin stdout to a file
	//char *out_prefix_final = strcat(output_prefix,reducedLPPOD);
	//char *finalname = strcat(out_prefix_final,".prelparse");
	fgetpos(stdout, &pos);
	fd = dup(fileno(stdout));

	if((stream = freopen(output_file, "w", stdout)) == NULL)
		exit(-1);


	int nr_stable_models = 0;

	if (!t.use_interactive_priorities) {

		while (!end) {
			found = g.model();
			nr_stable_models++;
			cout << "[Generator] Stable Model: " << nr_stable_models << endl;
			g.printAnswer();
			//cout << "************************" << endl;
			if (!found) {
				//cout << "!found" << endl;
				end = true;
			} else {
				set_compute(t);

				found = t.model();
				//cout << "Printing tester model" << endl;
				//t.printAnswer();
				//cout << "************************" << endl;
				t.model_calls++;
				if (!found) {

					cout << "Answer: " << ++models_found << endl;
					//cout << "before printAnswer" << endl;
					g.printAnswer();
					//cout << "after printAnswer" << endl;
				} else {
					// if only one model is desired, continue iteration
					if (models == 1) {
						//	    cerr << "shoudn't happen" << endl;
						//	    exit(1);
						do {
							update_tester(t);
							t.model_calls++;
						} while (t.model());
						end = true;
						cout << "before print best model" << endl;
						cout << "Answer: " << ++ models_found << endl;
						t.print_best_model();
						cout << "after printbest model" << endl;
					}
				}
			}
			if (models && models_found >= models) {
				end = 1;
			}
		}
	} else {
		interactive_models(g, t);
	}
	//Close file and restore standard output to stdout - which should be the terminal
	fflush(stdout);
	dup2(fd, fileno(stdout));
	close(fd);
	clearerr(stdout);
	fsetpos(stdout, &pos);

	cout << "Tester calls: " << t.model_calls << endl;
	return 0;
}
