//
// Julia Generator
// Programm zum Erzeugen von Fraktalen nach der Formel von Gaston Julia
//
// Copyright by Great!Soft on 01th Oct 2000
//

// Standard
#include <stdio.h>
#include <stdlib.h>
// Be Interface
#include <Alert.h>
#include <Application.h>
#include <Beep.h>
#include <Bitmap.h>
#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <ColorControl.h>
#include <File.h>
#include <ListItem.h>
#include <ListView.h>
#include <Message.h>
#include <Messenger.h>
#include <MidiSynth.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <OutlineListView.h>
#include <ScrollView.h>
#include <Slider.h>
#include <StatusBar.h>
#include <String.h>
#include <StringView.h>
#include <TextView.h>
#include <View.h>
#include <Window.h>

#define lbl1 B_FOLLOW_LEFT|B_FOLLOW_TOP
#define lbl2 B_WILL_DRAW
#define tx1 B_WILL_DRAW|B_PULSE_NEEDED

const int Parameter = 4; // 5 Parameter
const int maxx = 799;
const int maxy = 599;
const uint32 MSG_btnCalc = 1024;
const uint32 MSG_btnBsp = 1025;
const uint32 MSG_btnR = 1026;
const uint32 MSG_btnL = 1027;
const uint32 MSG_btnO = 1028;
const uint32 MSG_btnU = 1029;
const uint32 MSG_chkColor = 1030;
const uint32 MSG_chkInvert = 1031;

//
// JuliaView class
//
class JuliaView : public BView {
	public:
						JuliaView(BRect frame);
		virtual void	Draw(BRect updateRect);
		virtual void	SetzePunkt(int x, int y);
};

//
// JuliaView constructor
//
JuliaView::JuliaView(BRect frame) : BView(frame, "JuliaView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
}

//
// JuliaView::Draw
//
void JuliaView::Draw(BRect updateRect)
{
}

void JuliaView::SetzePunkt(int x, int y)
{
	BRect punkt;
	punkt.top = 105+y;
	punkt.bottom = 105+y;
	punkt.left = 10+x;
	punkt.right = 10+x;
	FillRect(punkt, B_SOLID_HIGH);
}
//
// JuliaWindow class
//
class JuliaWindow : public BWindow {
	public:
						JuliaWindow(BRect frame);
		virtual void	MessageReceived(BMessage *message);
		virtual bool	QuitRequested();
	private:
		JuliaView		*jv;
		void			Info(BString info);
		int				example;
		char			useColor;
		char			invert;
		BStringView		*lblPar[Parameter];
		BTextView		*txtPar[Parameter];
		BBox			*boxPar[Parameter];
		BCheckBox		*chkColor;
		BCheckBox		*chkInvert;
		BButton			*btnCalc;
		BButton			*btnBsp;
		BButton			*btnR;
		BButton			*btnL;
		BButton			*btnO;
		BButton			*btnU;
		BStringView		*lblInfo;
		BBox			*boxInfo;
		void			On_btnCalc();
		void			On_btnBsp();
		void			On_chkColor();
		void			On_chkInvert();
		void			notdevyet();
};

void JuliaWindow::Info(BString info)
{
	BRect redraw = lblInfo->Bounds();
	lblInfo->SetText(info.String());
	lblInfo->Draw(redraw);
}

void SetT(BRect &sett, int linenum)
{
	sett.left = 10;
	sett.right = 90;
	sett.top = 10+linenum*25;
	sett.bottom = sett.top + 15;
}

void IncT(BRect &inct, int width=90, int space=20)
{
	inct.left = inct.right + space;
	inct.right = inct.left + width;
}

void Umrahmen(BRect &umr)
{
	umr.top -= 5;
	umr.bottom += 5;
	umr.left -= 5;
	umr.right += 5;
}

//
// JuliaWindow constructor
//
JuliaWindow::JuliaWindow(BRect frame) : BWindow(frame, "Julia", B_TITLED_WINDOW, 0)
{
	BRect r=Bounds();
	int i,j;
	BString Label;
	
	example=0;
	
	// Labels, Textfelder und Umrahmungen erstellen
	BRect t=r;
	SetT(t,0);
	for (j=0;j<=Parameter;j++)
	{
		switch (j)
		{
			case 0:	Label="Link";		break;
			case 1: Label="Recht";		break;
			case 2:	Label="Ober";		break;
			case 3:	Label="Unter";		break;
			case 4:	Label="Suchtiefe:";	break;
		}
		if (j<4) Label << "er Rand:";
		lblPar[j] = new BStringView(t, "lblPar", Label.String(), lbl1, lbl2);
		AddChild(lblPar[j]);
		IncT(t);
	}
	SetT(t,1);
	for (i=0;i<=Parameter;i++)
	{
		BRect u=t;
		u.OffsetTo(B_ORIGIN);
		t.left += 5;
		if (i==Parameter) t.right -= 40;
		txtPar[i] = new BTextView(t, "txtPar", u, 0, tx1);
		AddChild(txtPar[i]);
		txtPar[i]->SetDoesUndo(true);
		BRect box=t;
		Umrahmen(box);
		boxPar[i] = new BBox(box);
		AddChild(boxPar[i]);
		IncT(t);
	}
	txtPar[0]->MakeFocus(true);
	chkColor = new BCheckBox(t, "color", "Färbig", new BMessage(MSG_chkColor));
	AddChild(chkColor);
	chkColor->SetValue(1);
	t.top -= 20;
	t.bottom -= 20;
	chkInvert = new BCheckBox(t, "invert", "Invertieren", new BMessage(MSG_chkInvert));
	AddChild(chkInvert);
	
	// "Berechnen" Button erstellen
	SetT(t,2);
	t.top += 5;
	btnCalc = new BButton(t, "calc", "Berechnen", new BMessage(MSG_btnCalc));
	btnCalc->MakeDefault(true);
	AddChild(btnCalc);
	
	// "Beispiele" Button erstellen
	IncT(t);
	btnBsp = new BButton(t, "bsp", "Beispiele", new BMessage(MSG_btnBsp));
	AddChild(btnBsp);
	
	// Navigations-Buttons erstellen
	IncT(t,30);
	btnL = new BButton(t, "l", "<", new BMessage(MSG_btnL));
	AddChild(btnL);
	IncT(t,30,10);
	btnR = new BButton(t, "r", ">", new BMessage(MSG_btnR));
	AddChild(btnR);
	IncT(t,30,10);
	btnO = new BButton(t, "o", "^", new BMessage(MSG_btnO));
	AddChild(btnO);
	IncT(t,30,10);
	btnU = new BButton(t, "u", "`´", new BMessage(MSG_btnU));
	AddChild(btnU);
	
	// Zeichenfläche???
/*	bm = new BBitmap(BRect(50,50,370,250), B_CMAP8, true);
	AddChild(bm); */
	
	// Infoleiste erstellen
	t.top = maxy+120;
	t.bottom = maxy+135;
	t.left = 10;
	t.right = r.right - 10;
	lblInfo = new BStringView(t, "lblInfo", "Bereit.", lbl1, lbl2);
	AddChild(lblInfo);
	Umrahmen(t);
	boxInfo = new BBox(t);
	AddChild(boxInfo);
	
	// Fenster instanzieren und anzeigen (u.a. notwendig, für Zugriff auf JuliaView)
	jv = new JuliaView(Bounds());
	AddChild(jv);
	Show();
}

//
// JuliaWindow QuitRequested
//
bool JuliaWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void colortranslate(int col, char &rot, char &gruen, char &blau, int inv)
{
	int mxtr = 16, mxtrq, subfrom;
	inv==0 ? subfrom=0 : subfrom=255;
	mxtrq=mxtr*mxtr;
	rot = subfrom-col/mxtrq;
	gruen = subfrom-(col - rot*mxtrq);
	blau = subfrom-(col - rot*mxtrq - gruen*mxtr);
}

void JuliaWindow::On_btnCalc()
{
	// Vom Benutzer eingegebene Parameter
	float xi,xa,yi,ya;
	int t,tm,loop,i,j;
	char rp,gp,bp; // Farbanteile
	BString errmsg=""; // Fehlermeldung
	
	// Von der Routine benutzte Variablen
	float dx,dy,cx,cy,xq,yq,x,y;
	
	// Holen und Überprüfen der Parameter
	char Eingabe[20];
	for (loop=0;loop<=Parameter;loop++)
	{
		txtPar[loop]->GetText(0,19,Eingabe); // Hole Text aus Textfeld
		switch (loop)
		{
			case 0:	xi = atof(Eingabe);	break;
			case 1:	xa = atof(Eingabe);	break;
			case 2:	ya = atof(Eingabe);	break;
			case 3:	yi = atof(Eingabe);	break;
			case 4:	tm = atoi(Eingabe); break;
		}
	}
	if (xi>=xa) errmsg = "Rechter Rand muss größer als linker Rand sein!";
	if (yi>=ya) errmsg = "Unterer Rand muss größer als oberer Rand sein!";
	if (tm<=5) errmsg = "Suchtiefen unter 6 haben keinen Sinn!";
	if (errmsg.String()=="")
	{
		// Bereich löschen
		jv->SetHighColor(255,255,255,255);
		jv->FillRect(BRect(10,105,10+maxx,105+maxy),B_SOLID_HIGH);
		// Berechnen
		Info("Rechne...");
		dx=(xa-xi)/320;
		dy=(ya-yi)/200;
		cx=xi;cy=ya;
		for (j=0;j<=maxy;j++)					// Zeilen
		{
			for (i=0;i<=maxx;i++)				// Spalten
			{
				x=0;y=0;xq=0;yq=0;t=0;
				do {
					y=2*x*y-cy;x=xq-yq-cx;
					xq=x*x;yq=y*y;t++;
				} while ((xq+yq<4) && (t<tm));	// x² + y² muss größer als 4 und Suchtiefe erreicht sein
				if (t!=tm) {
					if ((t && 1)==1) {
						if (useColor) {
							colortranslate(t,rp,gp,bp,invert);
						}
						else {
							rp=t;
							gp=t;
							bp=t;
							if (invert) {
								rp=!rp;
								gp=!gp;
								bp=!bp;
							}
						}
						jv->SetHighColor(rp,gp,bp,255);
						jv->SetzePunkt(i,j);	// Punkt an gefundener Stelle setzen
					}
				}
				cx=cx+dx;
			}
			cx=xi;
			cy=cy-dy;
		}
		Info("Bereit.");
		beep();
	}
	else
	{
		if (errmsg!="x")
		{
			BAlert *hinweis = new BAlert("Hinweis", errmsg.String(), "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
			hinweis->Go();
		}
	}
}

void JuliaWindow::On_btnBsp()
{
	switch (example)
	{
		case 0:
		{
			txtPar[0]->SetText("-1.8");
			txtPar[1]->SetText("0.2");
			txtPar[2]->SetText("3.65");
			txtPar[3]->SetText("1.25");
			txtPar[4]->SetText("60");
			Info("Dies ist das 'Grund'-Apfelmännchen, indem es viel zu erforschen gibt.");
		}
		break;
		case 1:
		{
			txtPar[0]->SetText("0.74448");
			txtPar[1]->SetText("0.74591");
			txtPar[2]->SetText("0.11339");
			txtPar[3]->SetText("0.11196");
			txtPar[4]->SetText("240");
			Info("Dies ist ein kleines Beispiel, sehr weit im Inneren des Apfelmannes.");
		}
		break;
		case 2:
		{
			txtPar[0]->SetText("0.7425");
			txtPar[1]->SetText("0.74825");
			txtPar[2]->SetText("0.10067");
			txtPar[3]->SetText("0.09621");
			txtPar[4]->SetText("180");
			Info("Hier erkennt man unter anderem die Ähnlichkeit der Objekte.");
		}		
	}
	example++;
	if (example>2) example=0;
}

void JuliaWindow::On_chkColor()
{
	useColor=!useColor;
}

void JuliaWindow::On_chkInvert()
{
	invert=!invert;
}

void JuliaWindow::notdevyet()
{
	beep();
	Info("Funktion noch nicht implementiert!");
}

//
// JuliaWindow MessageReceived
//
void JuliaWindow::MessageReceived(BMessage *message) {
	switch(message->what) {
		case MSG_btnCalc:		On_btnCalc();	break;
		case MSG_btnBsp:		On_btnBsp();	break;
		case MSG_chkColor:		On_chkColor();	break;
		case MSG_chkInvert:		On_chkInvert();	break;
		case MSG_btnR:			notdevyet();	break;
		case MSG_btnL:			notdevyet();	break;
		case MSG_btnO:			notdevyet();	break;
		case MSG_btnU:			notdevyet();	break;
		
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

//
// JuliaApp class
//
class JuliaApp : public BApplication {
	public:
						JuliaApp();
	private:
		JuliaWindow		*theWindow;
};

//
// JuliaApp constructor
//
JuliaApp::JuliaApp() : BApplication("application/x-vnd.Julia") {
	BRect windowRect;
	
	windowRect.Set(40,45,maxx+60,maxy+190);
	theWindow = new JuliaWindow(windowRect);
}

//
// main
//
int main()
{
	JuliaApp *theApp;
	theApp = new(JuliaApp);
	theApp->Run();
	delete theApp;
}
