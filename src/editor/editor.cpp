/**
 Xtase - fgalliat @Oct 2020

 Original code from : https://codeworldtechnology.wordpress.com/2009/07/02/program-to-make-a-text-editor-in-c/
*/

#include<stdio.h>            //stdio.h used for input or output operations
#include<string.h>           //used for strcpy function
#include<stdlib.h>           //used for system("clear") function
#include<termios.h>          //used for getch function
#include<unistd.h>           //used for getch function

#define TXT_LINE_LENGTH 200
#define FILE_NAME_LENGTH 20

#ifndef uint8_t
 #define uint8_t char
#endif

/*
 * Ideas section 
 *  - limit ttyWidth (faster cls)
 *  - windows of chars
 *  - simple textBox (even if no window support)
 *   - printBox(int x, int y, int w, int h, const char* txt)
 *  - reverse video attribute for status line
 * 
 *  - Fs abstraction
 * 
 * Todo section
 *  - paginate text -> ~window
 * 
 */


// ***** Input / Output routines section *****

#define TTY_SCREEN_WIDTH 80
#define TTY_SCREEN_HEIGHT 25
#define TTY_SCREEN_SUPPORT 1

#if TTY_SCREEN_SUPPORT
  char emptyLine[TTY_SCREEN_WIDTH+1];
#endif

void cls();
void gotoXY(int x, int y); // 0 based
int getch();
// void gets(char* dest)

void textBox(int x, int y, int w, int h, const char* text);
void box(int x, int y, int w, int h);

void t_write(char ch) { printf("%c", ch); }

void textBox(int x, int y, int w, int h, const char* text) {
    gotoXY(x,y);
    int len = strlen(text);
    int xx = x, yy = y;
    char c;
    for(int i=0; i < len; i++) {
        c = text[i];
        if ( c != '\n' && xx < (x+w) ) {
            t_write( c );
            xx++;
        } else if ( c == '\n' && yy < (y+h) ) {
            yy++;
            xx=x;
            gotoXY(xx,yy);
        }
    }
}

void box(int x, int y, int w, int h) {
    gotoXY(x,y);
    for(int xx=x; xx < (x+w); xx++) {
        t_write('o');
    }
    gotoXY(x,y+h-1);
    for(int xx=x; xx < (x+w); xx++) {
        t_write('o');
    }

    for(int yy=y; yy < (y+h); yy++) {
        gotoXY(x,yy);
        t_write('o');
    }
    for(int yy=y; yy < (y+h); yy++) {
        gotoXY(x+w-1,yy);
        t_write('o');
    }
}


void cls() {
    #if TTY_SCREEN_SUPPORT
      // FIXME : do better
      memset(emptyLine, 0x20, TTY_SCREEN_WIDTH); // makes fp not NULL !!!
      emptyLine[TTY_SCREEN_WIDTH-1] = '\n';
      emptyLine[TTY_SCREEN_WIDTH] = 0x00;

      gotoXY(0,0);
      for(int y=0; y < TTY_SCREEN_HEIGHT; y++) {
          printf( "%s", emptyLine );
      }
      gotoXY(0,0);
    #else
      system("clear");
    #endif
}

// ^[<row>;<col>H -> set cursor position -> 0based
// ^J             -> cls

// 0 based
void gotoXY(int x, int y) {
    printf("%c[%d;%dH", (char)27, y, x);
}


int getch()                            //getch function definition
{
    struct termios oldt, newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}

// ******** compatibility section **************
void dirtyGets(char* dest) { // gets(char* dst) - replacement
  size_t size = TXT_LINE_LENGTH;
  getline( &dest, &size, stdin);
}

#define gets(a) dirtyGets(a);
// ******* /compatibility section/ *************

// **** /Input / Output routines section/ ****

struct dll                    //definition of the structure double linked list
{
    char s[TXT_LINE_LENGTH];          //character array
    int index;
    struct dll*prev,*next;            //pointers to next and previous nodes
};


void addnode(char t[],struct dll *q);
void delnode(struct dll *p);
void clealist(void);

void showhelp(void);                    //function prototypes
void addline(struct dll *temp);
void editnode(struct dll *p);
void inp(void);
void printlist(int lineNum);
void closer(void);
void edit(void);
void save(void);

struct dll *head;                        //header node declaration
char file[FILE_NAME_LENGTH];

FILE *fp=NULL;                           //file pointer declaration

main()
{
    char c;
    fp=NULL;

    head=(struct dll*)malloc(sizeof(struct dll));            //header node memory allocation
    (head->next)=(head->prev)=NULL;                //initialization
    (head->index)=0;

    while(1)                //infinite while loop for editing multiple number of tiles
    {

        cls();                        //clearing the screen

        //Displaying editor options
        printf("\nThis Editor provides the following options \n");
        printf("R :opens a file and reads it into a buffer\n   If file doesnot exist creates a new file for editing\n");
        printf("E :edit the currently open file\n");
        printf("X :closes the current file and allows you to open another file\n");
        printf("Q :quit discarding any unsaved changes\n");

        c=getch();                            //taking user input
        switch(c)                            //testing with switch
        {
            case 'r' :
            case 'R' :
                inp();
                break;
            case 'e' :
            case 'E' :
                edit();
                break;
            case 'x' :
            case 'X' :
                closer();
                break;
            case 'q' :
            case 'Q' :
                cls();
                exit(1);
                break;
        }
    }
} //end of main



void addnode(char t[],struct dll *q)        //function to add a new node after a node q
{
    struct dll*p=(struct dll*)malloc(sizeof(struct dll));
    struct dll *temp=q->next;
    strcpy(p->s,t);
    p->prev=q;
    p->next=q->next;

    if((q->next)!=NULL)        //adding the node to the list by manipulating pointers accordingly
    {
        ((q->next)->prev)=p;
        while(temp!=NULL)
        {
            (temp->index)++;        //incrementing the index of the later nodes
            temp=temp->next;
        }
    }
    q->next=p;
    p->index = q->index + 1;                    //setting the index of the new node
}



void delnode(struct dll *p)                    //function to delete a node
{
    struct dll *temp=p->next;
    ((p->prev->next))=p->next;
    if(p->next!=NULL)
    {
        ((p->next)->prev)=p->prev;
        while(temp!=NULL)
        {
            (temp->index)--;        //decrementing the index of the later nodes
            temp=temp->next;
        }
    }
    free(p);                        //freeing ht memory of the deleted node
}



void clearlist(void)                        //function to clear the list
{
    while(head->next!=NULL)
        delnode(head->next);                    //deleting all nodes except head
}

// =================================================================================


void editnode(struct dll *p)                    //function to edit a line
{
    printf("\nThe original line is :\n%s",p->s);
    printf("\nEnter the new line :\n");
    gets(p->s);                                 //taking the new line input
    printf("\nLine edited\n");
}


void printlist(int lineNum)            //function to print all the lines stored in the buffer
{
    #define NO_LINE -1

    struct dll *temp=head;
    char cursor[3] = { 0x00, 0x00, 0x00 };
    cls();
    while(temp->next!=NULL)
    {
        temp=temp->next;
        //printf("%d %s\n",temp->index,temp->s);            //printing the lines on the screen

        if ( lineNum != NO_LINE ) {
            if ( lineNum == temp->index ) {
                sprintf(cursor, ">");
            } else {
                sprintf(cursor, " ");
            }
        }

        printf("%s %d %s", cursor ,temp->index,temp->s);            //printing the lines on the screen
    }
}



void closer(void)                //function to close the file orened for editing
{
    if(fp==NULL)
        return;
    fclose(fp);
    fp=NULL;
    clearlist();                 //the list is also cleared at this point
}



void inp(void)                  // read file to buffer
{
    struct dll *buff=head;                        //temporaty variable
    char c;
    char buf[TXT_LINE_LENGTH];                    //array to store input line

    if(fp!=NULL)                                  //checking for files opened earlier
    {
        printf("\nThere is another file open it will be closed\ndo you want to continue ?(Y/n):");
        c=getch();
        if(c=='n'||c=='N')
            return;
        else
            closer();
    }

    fflush(stdin);
    printf("\nEnter the file name you want to open :");
    scanf("%s",file);
    getchar();
    fflush(stdin);
    clearlist();

    fp=fopen(file,"r");                        //opening the specified file
    if(fp==NULL)                         //checking if the file previously exists
    {
        printf("\nThe file doesnot exist do you want to create one?(Y/n) :");
        c=getchar();
        getchar();
        if(c=='N'||c=='n')
            return;
        else
        {
            fp=fopen(file,"w");                //creating new file
            edit();
            return;
        }
    }

    if(feof(fp))
        return;

    while((fgets(buf,TXT_LINE_LENGTH+1,fp))!=NULL)                //taking input from file
    {
        addnode(buf,buff);
        buff=buff->next;
    }
    edit();                            //calling the edit function
}


void showText(int lineNum) {
    cls();
    printlist( lineNum );                       //printing the entire buffered list
}


void edit(void)                            //the edit function
{
    struct dll *temp=head->next;          //pionter used to mark the current position during traversing
    char c,d;

    cls();                        //clearing the screen

    if(fp==NULL)                        //checking for files previously open
    {
        printf("\nNo file is open\n");
        return;
    }

    // printf("\nThe file contents will be displayed along with the line number\npress any key\n");
    // getch();

    if(temp!=NULL) {
        // printf("You are at line number %d\n",temp->index);    //printing the line number of control
    }
    else {
        temp=head;
    }

    // showText( temp->index );                       //printing the entire buffered list
    // showhelp();                    //prints the list of commands available

    bool dispHelpReq = true;

    while(1)                        //infinite loop for multiple command usage
    {
        showText( temp->index );
        if ( dispHelpReq ) { showhelp(); dispHelpReq = false; }
        c=getch();

        switch(c)                        //switch -->condition checkig
        {
            case 'c' :
            case 'C' :
                editnode(temp);            //edit the current line pointed to by temp
                break;

            case 'p' :
            case 'P' :                    //move to the previous line
                if(temp==head)
                {
                    printf("\nFile empty\n");    //message displayed if list is empty
                    break;
                }
                if(temp->prev!=head)
                {
                    temp=temp->prev;
                    showText( temp->index );
                    // printf("\nYou are at line number %d\n",temp->index);
                }
                else                //message display if already at first line
                    printf("\nalready at first line\n");
                break;

            case 'n' :
            case 'N' :                    //move to the next line
                if(temp->next!=NULL)
                {
                    temp=temp->next;
                    showText( temp->index );
                    // printf("\nYou are at line number %d\n",temp->index);
                }
                else if(temp==head)
                    printf("\nFile empty\n");        //message printed if list is empty
                else
                    printf("\nalready at last line\n");//message printed if already at last line
                break;

            case 'a' :
            case 'A' :                          //adding a new line after node ponted by temp
                addline(temp);                  //addline function takes input and creates a new node via addnode function
                temp=temp->next;
                showText( temp->index );
                // printf("\nYou are at line number %d",temp->index);
                break;

            case 'h' :
            case 'H' :                    //HELP command displays the list of available commmands
                // showText( temp->index );
                // showhelp();
                dispHelpReq = true;
                break;

            case 'v' :
            case 'V' :                    //printing the entire list via printlist function
                // showText( temp->index );
                // printf("\nYou are at line number %d",temp->index);
                break;

            case 'D' :
            case 'd' :                    //deleting a line pointed to by temp
                if(temp==head)                //checking if list is empty
                {
                    printf("\nFile empty\n");
                    break;
                }
                temp=temp->prev;
                delnode(temp->next);            //deleting the node
                showText( temp->index );       //printing the list
                printf("\nLine deleted\n");
                if(temp->index) {
                    // printf("\nYou are currently at line number %d\n",temp->index);
                }
                if(((temp->prev)==NULL)&&(temp->next)!=NULL) {
                    temp=temp->next;
                }
                else if((temp==head)&&((temp->next)==NULL)) {
                    printf("\nFile empty\n");      //printing message if list is empty
                }
                break;

            case 'X' :
            case 'x' :                    //exit the editor to main menu
                printf("\nDo you want to save the file before exiting?(y/N) :");
                d=getch();                //warning for saving before exit
                if(d=='y'||d=='Y')
                    save();
                closer();
                return;
                break;

            case 's' :
            case 'S' :                    //saving and exitting
                save();
                closer();
                return;
                break;
        }

    }

}


void addline(struct dll *temp)                    //adding a new line via input from user
{
    char buff[TXT_LINE_LENGTH];
    printf("\nenter the new line :\n");
    gets(buff);                                //taking the new line
    // buff : contains trailling '\n'
    addnode(buff,temp);                        //ceatng the new node
}


void save(void)                            //function to save the file
{
    struct dll *temp=head->next;
    fclose(fp);
    fp=fopen(file,"w");                        //opening the file in write mode

    while(temp!=NULL)
    {
        fprintf(fp,"%s",temp->s);                    //writing the linked list contents to file
        temp=temp->next;
    }
}


void showhelp(void)                        //function to print the list of editer commands
{

    #if TTY_SCREEN_SUPPORT
    const char* helpTxt = 
    "                                           \n"\
    "                  Editor commands\n"\
    "                                           \n"\
    " C :edit the current line                  \n"\
    " P :move one line up                       \n"\
    " N :move one line down                     \n"\
    " D :delete the current line                \n"\
    " V :display the contents of the buffer     \n"\
    " A :add a line after the line at which you are navigating\n"\
    " S :save changes and exit to main menu     \n"\
    " X :exit discarding any changes            \n"\
    " H :show the list of commands              \n"\
    "                                           \n";

    textBox(8,3,40,14, helpTxt);
    box(7,2, 45, 15);
    #else
    gotoXY( 8, 2 ); int i = 2;
    printf("--------------------------------------------------------------\n");
    gotoXY( 8, ++i ); printf("|                   Editor commands\n");
    gotoXY( 8, ++i ); printf("|                                           \n");
    gotoXY( 8, ++i ); printf("| C :edit the current line                  \n");
    gotoXY( 8, ++i ); printf("| P :move one line up                       \n");
    gotoXY( 8, ++i ); printf("| N :move one line down                     \n");
    gotoXY( 8, ++i ); printf("| D :delete the current line                \n");
    gotoXY( 8, ++i ); printf("| V :display the contents of the buffer     \n");
    gotoXY( 8, ++i ); printf("| A :add a line after the line at which you are navigating\n");
    gotoXY( 8, ++i ); printf("| S :save changes and exit to main menu     \n");
    gotoXY( 8, ++i ); printf("| X :exit discarding any changes            \n");
    gotoXY( 8, ++i ); printf("| H :show the list of commands              \n");
    gotoXY( 8, ++i ); printf("--------------------------------------------------------------\n");
    // getch();
    #endif
}