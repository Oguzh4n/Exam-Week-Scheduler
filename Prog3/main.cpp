using namespace std;
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>

/*VON:
 * JAN BOTHMANN
 * OGUZHAN YÜKSEL
 */

// CSV ANMELDUNGEN : mtknr;stg;pversion;pnr;pform;dtxt;psem
// CSV PRUEFUNGEN  : stg;vert;pversion;pnr;pdtxt;ppruefer;Prüfer 1;zweitpruefer;Prüfer 2;pdauer;pform;psem;angeboten

class Ressource {
public:
    int tag; // Tage gehen von 1-10
    int anzStunden = 10;
    int kapazitaet;
};

class Ergebnis {
public:
    int tag;
    int stundeAnfang;
    int stundeEnde;
    int minute = 0;
    string raumnummer;
    string stg;
    string pnr;
    string pdtxt;
};

class Raum {
public:
    string hoersaal;
    string raumnummer;
    string kapazitaet;
    int kapa;
    vector<Ressource> ressource;
};

class Anmeldung {
public:
    string mtknr;
    string stg; // 3
    string pversion; // 1
    string pnr; // 2
    string pform;
    string dtxt;
    string psem;
};

class Pruefung {
public:
    string stg; // 3
    string vert;
    string pversion; // 1
    string pnr; // 2
    string pdtxt;
    string ppruefer;
    string Pruefer1;
    string zweitpruefer;
    string Pruefer2;
    string pdauer;
    string pform;
    string psem;
    string angeboten;
    int anzahlPrueflinge;
    int dauerinH;

};

std::istream& lese_ein( std::istream& in, Anmeldung& a )
{
    getline(in, a.mtknr, ';');
    getline(in, a.stg, ';');
    getline(in, a.pversion, ';');
    getline(in, a.pnr, ';');
    getline(in,a.pform,';');
    getline(in,a.dtxt,';');
    getline(in,a.psem,'\n');
    return in;
}

std::istream& lese_ein1(std::istream& in, Raum& r )
{
    getline(in, r.hoersaal, ',');
    getline(in, r.raumnummer, ',');
    getline(in, r.kapazitaet, '\n');
    return in;
}

std::istream& lese_ein2( std::istream& in, Pruefung& p )
{
    getline(in, p.stg, ';');
    getline(in, p.vert, ';');
    getline(in, p.pversion, ';');
    getline(in,p.pnr,';');
    getline(in,p.pdtxt,';');
    getline(in,p.ppruefer,';');
    getline(in, p.Pruefer1, ';');
    getline(in,p.zweitpruefer,';');
    getline(in, p.Pruefer2, ';');
    getline(in,p.pdauer,';');
    getline(in,p.pform,';');
    getline(in,p.psem,';');
    getline(in,p.angeboten,'\n');
    return in;
}

void kapaBerechnen(vector <Raum> &r) {
    for (int i = 0; i < r.size(); ++i) {
        r[i].kapa = stoi(r[i].kapazitaet)/4;
    }
}

void rundeStunden(vector <Pruefung> &p) {
    for (int i = 0; i < p.size(); ++i) {
        if(!p[i].pdauer.empty() && (p[i].pdauer.compare("0") != 0)) {
            if((stoi(p[i].pdauer)== 60) || (stoi(p[i].pdauer)== 120) || (stoi(p[i].pdauer)== 180) || (stoi(p[i].pdauer)== 240)
               || (stoi(p[i].pdauer)== 300)) {
                p[i].dauerinH = stoi(p[i].pdauer)/60;
            }
            else{
                p[i].dauerinH = stoi(p[i].pdauer)/60 +1;
            }
        }
        if((p[i].pdauer.compare("0") == 0)) {
            p[i].dauerinH = 0;
        }
    }
}
void berechneZuteilung(vector<Raum> &raumListe, vector<Pruefung> &pruefungListe, vector<Ergebnis> &ergebnisListe) {
    std::ofstream ergebnisFile("Ergebnisliste.csv");
    ergebnisFile << "studiengang" << ";" << "pnr" << ";" << "raumnummer" << ";" << "pdtxt" << ";" << "tag" << ";" << "startzeit" << ";" << "stoptzeit" << endl;
    int i = 0;
    first:
    for (i; i < pruefungListe.size(); ++i) {
        if (pruefungListe[i].anzahlPrueflinge == 0) {
            cout << "Die Pruefung: " << pruefungListe[i].pdtxt << ", Studiengang: " << pruefungListe[i].stg <<
                 ", Pruefungsversion: " << pruefungListe[i].pversion << ", hat keine Prueflinge" << endl;
        }
        else if(pruefungListe[i].dauerinH == 0) {
            cout << "Die Pruefung: " << pruefungListe[i].pdtxt << ", Studiengang: " << pruefungListe[i].stg <<
                 ", Pruefungsversion: " << pruefungListe[i].pversion << ", hat 0 Stunden dauer" << endl;
        }
        else {
            for (int j = 0; j < raumListe.size(); ++j) { // Jede Prüfung mit allen Räumen vergleichen
                for (int k = 0; k < raumListe[j].ressource.size(); ++k) { // Von jedem Raum die Tage bekommen
                    if (raumListe[j].ressource[k].anzStunden >= pruefungListe[i].dauerinH) { // Tag hat freie Stunden
                        second:
                        ergebnisListe[j].tag = raumListe[j].ressource[k].tag;
                        ergebnisListe[j].raumnummer = raumListe[j].raumnummer;
                        ergebnisListe[j].pdtxt = pruefungListe[i].pdtxt;
                        ergebnisListe[j].stg = pruefungListe[i].stg;
                        ergebnisListe[j].pnr = pruefungListe[i].pnr;
                        // wenn es zu weniger Leute als Anz max Plätze gibt
                        if ((pruefungListe[i].anzahlPrueflinge - raumListe[j].kapa) < 0) {
                            raumListe[j].ressource[k].kapazitaet -= pruefungListe[i].anzahlPrueflinge;
                            pruefungListe[i].anzahlPrueflinge = 0;
                        }
                            // wenn es mehr/gleich viel Anz Prüflinge gibt zum Zimmer
                        else {
                            pruefungListe[i].anzahlPrueflinge -= raumListe[j].kapa; // kann negativ werden
                        }
                        ergebnisListe[j].stundeAnfang = 10 - raumListe[j].ressource[k].anzStunden;
                        raumListe[j].ressource[k].anzStunden -= pruefungListe[i].dauerinH;
                        ergebnisListe[j].stundeEnde = ergebnisListe[j].stundeAnfang + pruefungListe[i].dauerinH;
                        /*cout << "von: " << ergebnisListe[j].stundeAnfang << " bis : " << ergebnisListe[j].stundeEnde
                             << ", tag: " << ergebnisListe[j].tag << ", Raum: " << raumListe[j].raumnummer
                             << ", Pruefung: " <<
                             pruefungListe[i].pdtxt << endl;*/

                        // in CSV rein:
                        ergebnisFile << ergebnisListe[j].stg << ";" << ergebnisListe[j].pnr << ";" << ergebnisListe[j].raumnummer << ";" << ergebnisListe[j].pdtxt << ";" << ergebnisListe[j].tag << ";" << ergebnisListe[j].stundeAnfang << ";" << ergebnisListe[j].stundeEnde<< endl;
                        // 1 stunde pause
                        raumListe[j].ressource[k].anzStunden -= 4;

                        if (pruefungListe[i].anzahlPrueflinge == 0) { // alle eingeplant
                            i++;
                            goto first; // nächste Prüfung
                        } else if (raumListe[j].ressource[k].anzStunden >=
                                   pruefungListe[i].dauerinH) { // Selber Tag noch Platz hat
                            goto second;
                        }
                    } else { // Tag hatte keine freie Stunde oder ( alle Tage voll -> implizit nächster Raum )
                        continue;
                    }
                }
            }
        }
    }
}


int main() {
    // file
    vector<Raum> raumListe(54);
    vector<Pruefung> pruefungListe(544);
    vector<Anmeldung> anmeldungListe(9621);
    vector<Ergebnis> ergebnisListe(54); // genauso wie Raum
    vector<Ressource> ressourceListe(10); // da 10 tage

    ifstream input;
    input.open("Anmeldungen_WS2019_KL.csv");
    if(!input.is_open()) {
        cerr << "Fehler beim Oeffnen der Datei!" << endl;
    }
    else {
        while (!input.eof()) {
            for(int i = 0; i < anmeldungListe.size(); i++) {
                lese_ein(input, anmeldungListe[i]);
            }
        }
    }
    input.close();

    // 2 file
    input.open("Raumliste.csv");
    if(!input.is_open()) {
        cerr << "Fehler beim Oeffnen der Datei!" << endl;
    }
    else {
        while (!input.eof()) {
            for(int i = 0; i < raumListe.size(); i++) {
                lese_ein1(input, raumListe[i]);
            }
        }

    }
    input.close();
    // 3 file
    input.open("Angebotene_Pruefungen_KL.csv");
    if(!input.is_open()) {
        cerr << "Fehler beim Oeffnen der Datei!" << endl;
    }
    else {
        while (!input.eof()) {
            for(int i = 0; i < pruefungListe.size(); i++) {
                lese_ein2(input, pruefungListe[i]);
            }
        }
    }
    input.close();

    // kapa berechenen
    kapaBerechnen(raumListe);

    // anz prüflinge pro Fach in Pruefung abgespeichert : int anzahlPrueflinge
    for (int i = 0; i < pruefungListe.size(); i++) {
        int anzPrueflinge = 0;
        for (int j = 0; j < anmeldungListe.size(); j++) {
            if((anmeldungListe[j].stg.compare(pruefungListe[i].stg) == 0) && (anmeldungListe[j].pversion.compare(pruefungListe[i].pversion) == 0) && (anmeldungListe[j].pnr.compare(pruefungListe[i].pnr) == 0)) {
                anzPrueflinge++;
            }
        }
        pruefungListe[i].anzahlPrueflinge = anzPrueflinge;
    }
    rundeStunden(pruefungListe);

    for(int i = 0; i < raumListe.size(); i++) {
        raumListe[i].ressource = ressourceListe;
    }

    //kapazitaet eines Raums für alle 10 tage
    for (int k = 0; k < raumListe.size(); ++k) {
        for (int i = 0; i < raumListe[i].ressource.size(); ++i) {
            raumListe[k].ressource[i].kapazitaet = raumListe[k].kapa;
        }
    }


    for(int i = 0; i < raumListe.size(); i++) {  // jeder Raum hat nun 10 Ressourcen und Ressource sind in Tage aufgeteilt
        for (int j = 0; j < raumListe[i].ressource.size(); ++j) { // raumListe[i].raumListe.size() == 10
            raumListe[i].ressource[j].tag = j + 1;
        }
    }
    berechneZuteilung(raumListe,pruefungListe,ergebnisListe);

    return 0;
}
