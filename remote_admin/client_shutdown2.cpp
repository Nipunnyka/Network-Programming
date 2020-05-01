#include<graphics.h>
#include<stdio.h>
#include<bits/stdc++.h>
//#include<conio.h>
//#include<iostream.h>
#include<math.h>
#define M 4

void mm(float a[M][M],float b[M])
{float c[M]={0,0,0,0};

 for(int i=0;i<M;i++)
  {for(int j=0;j<M;j++)
   {c[i]=c[i]+a[i][j]*b[j];
    }
  }
 for(int k=0;k<M;k++)
 {b[k]=c[k];}
}

void display(float m[M],float p1[M],float p2[M])
{int y=getmaxy();
line(m[0]+100,y-m[1]-100,p1[0]+100,y-p1[1]-100);
line(m[0]+100,y-m[1]-100,p2[0]+100,y-p2[1]-100);

}

void copy(float a[],float b[])
{
for(int i=0;i<M;i++)
   a[i]=b[i];
}
int main()
{//clrscr();
int gd=DETECT,gm;
initgraph(&gd,&gm,NULL);

float a[M][M]={{1,0,0,-5},{0,1,0,-8},{0,0,1,-9},{0,0,0,1}};
float m[M]={5,8,9,1};
float p1[M]={14,13,17};
float p2[M]={-3,17,4};

display(m,p1,p2);

mm(a,m);
mm(a,p1);
mm(a,p2);

float n[M],q1[M],q2[M];
copy(n,m);
copy(q1,p1);
copy(q2,p2);

// to fig2
a[0][0]=.723;a[0][1]=-.365;a[0][2]=-.585;a[0][3]=0;
a[1][1]=.847;a[1][2]=-.52;a[1][3]=0;
a[2][0]=.69;a[2][1]=.38;a[2][2]=.61;a[2][3]=0;
mm(a,n);
mm(a,q1);
mm(a,q2);
a[0][0]=1;a[0][1]=0;a[0][2]=0;
a[1][1]=1;a[1][2]=0;a[1][3]=0;
a[2][0]=0;a[2][1]=0;a[2][2]=1;
n[0]+=200;q1[0]+=200;q2[0]+=200;
display(n,q1,q2);

copy(n,m);
copy(q1,p1);
copy(q2,p2);

//to fig3
a[0][0]=.72;a[0][1]=-.365;a[0][2]=-.585;
a[1][0]=.69;a[1][1]=.38;a[1][2]=.61;
a[2][1]=-.89;a[2][2]=.52;
mm(a,n);
mm(a,q1);
mm(a,q2);

a[0][0]=1;a[0][1]=0;a[0][2]=0;
a[1][0]=0;a[1][1]=1;a[1][2]=0;
a[2][1]=0;a[2][2]=1;
n[1]+=200;q1[1]+=200;q2[1]+=200;

display(n,q1,q2);

copy(n,m);
copy(q1,p1);
copy(q2,p2);

//to fig4 
a[0][0]=.69;a[0][1]=.38;a[0][2]=.61;
a[1][0]=-.485;a[1][1]=.87;
a[2][0]=-.536;a[2][1]=-.297;a[2][2]=.789;

mm(a,n);
mm(a,q1);
mm(a,q2);

n[0]+=200;q1[0]+=200;q2[0]+=200;
n[1]+=200;q1[1]+=200;q2[1]+=200;

display(n,q1,q2);

copy(n,m);
copy(q1,p1);
copy(q2,p2);

getch();
closegraph();
restorecrtmode();
return 0;
}
