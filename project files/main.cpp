#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<math.h>
#include<ctype.h>
#include<graphics.h>

/*---------------------------CIZIMDEKI HER IKI CIZGI ARASI 40 PIXELI TEMSIL ETMEKTEDIR ------------------------
------------------------------------MERKEZ NOKTA KIRMIZI ILE BELIRTILMISTIR------------------------------------
-------------------------------CIZIM EKRANI YATAYDA 1280 PIXEL DUSEYDE ISE 720 PIXELDIR-------------------------*/

typedef struct kordinatlar
{
    int x;
    int y;
} Nokta;
int karakter_sayici(FILE *dosyaptr);

float iki_nokta_uzakligi(int x1,int y1,int x2,int y2);

void noktalari_matrisleme(char dosyadizgi[],Nokta *noktalar);

float spline_icin_nokta_al(float t,Nokta *noktalar,float geridonenx[1],float geridoneny[1],int nokta_adeti,int kosul);

int cember_duzenleme(int x_1,int y_1,int x_2,int y_2,int x_3,int y_3,float *geridonderen_dizi);
int main()
{
    //--------------------DOSYAYI OKUYUP MATRISLEME-------------------------------------

    //DOSYAYI OKUYUP BIR DIZGIYE ATAMA ISLEMI

    FILE *dosyaptr;
    dosyaptr=fopen("points.txt","r");
    //DOSYAYI KONTROL EDER
    if(dosyaptr==NULL)
    {
        printf("--------------------------------------------------------------------------\n");
        printf("----------------------------DOSYA ACILAMADI-------------------------------\n");
        printf("--------------------------------------------------------------------------\n");
    }
    //TXT ICINDE KAÇ TANE KARAKTER VAR HESAPLAR
    int karakter_sayisi=karakter_sayici(dosyaptr);

    /*BURADA ALINAN KARAKTER SAYISINA GÖRE DOSYA DIZGIYE ATAMA YAPIYORUZ
    SONRA ATANAN KARAKTERLER KULLANIM KOLAYLIGI ICIN BIR MATRISE ATANACAK*/
    char *dosyadizgi;
    dosyadizgi=(char *)malloc(karakter_sayisi*sizeof(char));

    fscanf(dosyaptr,"%s",dosyadizgi);
    fclose(dosyaptr);
    //DIZDIDEKI SAYILARI SAYAR
    Nokta *noktalar;
    int sayi_adeti=0;
    int l;
    for(l=0; l<karakter_sayisi; l++)
    {
        if(isdigit(dosyadizgi[l])==1)
        {
            if(dosyadizgi[l+1]=='}' || dosyadizgi[l+1]==',')
            {
                sayi_adeti++;
            }
        }
    }
    int nokta_adeti=ceil(sayi_adeti/2);
    //ELEMAN SAYISINA GORE NOKTALARA DINAMIK BELLEKTEN YER AYIRIR
    noktalar=(Nokta *)malloc(ceil(nokta_adeti)*sizeof(Nokta));

    //BURADA NOKTALARI KULLANIM KOLAYLIGI OLSUN DIYE MATRISE ATAYACAGIZ
    noktalari_matrisleme(dosyadizgi,noktalar);

    //------------------------------------------EN UZAK IKI NOKTA TESPITI----------------------------

    //EN UZAK IKI NOKTANIN TESPITI ICIN NOKTALARI FONKSIYONA YOLLAR
    int i,j;
    int en_uzak_x1,en_uzak_y1,en_uzak_x2,en_uzak_y2;
    float en_uzak_mesafe=0;
    for(i=0; i<nokta_adeti-1; i++)
    {
        for(j=1+i; j<nokta_adeti; j++)
        {

            float tmp=iki_nokta_uzakligi(noktalar[i].x,noktalar[i].y,noktalar[j].x,noktalar[j].y);
            if(en_uzak_mesafe<tmp)
            {
                en_uzak_mesafe=tmp;
                en_uzak_x1=noktalar[i].x;
                en_uzak_y1=noktalar[i].y;
                en_uzak_x2=noktalar[j].x;
                en_uzak_y2=noktalar[j].y;
            }
        }
    }
    //---------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------MERKEZ YARICAP HESAPLAMA-----------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------

    //MERKEZ NOKTASININ X'INI VE Y'SINI HESAPLAR
    float merkez_x=(en_uzak_x1+en_uzak_x2)/2;
    float merkez_y=(en_uzak_y1+en_uzak_y2)/2;
    float yaricap=sqrt(pow(merkez_x-en_uzak_x1,2)+pow(merkez_y-en_uzak_y1,2));

    //-----------------------------------------CEMBER HESAPLAMALARI -------------------------------------------

    /*CEMBERIN DISINDA NOKTA KALIYORMU ONU KONTROL EDECEGIZ EGER DISINDA NOKTA KALIYOR ISE
    FONKSIYONA YOLLAYARAK EN AZ TEMAS EDEN 2 NOKTA BULUP CIZDIRECEGIZ*/
    int degisim_gerekiyormu=0;
    int y;
    float geridonderen_dizi[3];
    for(y=0; y<nokta_adeti; y++)
    {
        int x_kare=pow(merkez_x-noktalar[y].x,2);
        int y_kare=pow(merkez_y-noktalar[y].y,2);
        float sonuc=sqrt(x_kare+y_kare);
        if(sonuc>yaricap)
        {
            degisim_gerekiyormu=cember_duzenleme(en_uzak_x1,en_uzak_y1,en_uzak_x2,en_uzak_y2,noktalar[y].x,noktalar[y].y,geridonderen_dizi);
            break;
        }
    }
    /*EGER USTTEKI CEMBER DEGISIM FONKSIYONUNA GIRMEZ ISE CEMBERDE BOS VE HATALI ATAMALAR YAPMASININ ONUNE
    GECMEK ICIN DEGISIM GEREKIYORMU KONTROLCUSUNU KOYDUK*/
    if(degisim_gerekiyormu==1)
    {
        merkez_x=geridonderen_dizi[0];
        merkez_y=geridonderen_dizi[1];
        yaricap=geridonderen_dizi[2];
    }
    //---------------------YARICAP MERKEZ X VE MERKEZ Y KORDINATLARINI YAZDIRMA-------------------------------
    printf("Merkez (x,y)=(%.2f,%.2f)\nYaricap uzunlugu= %.2f ",merkez_x,merkez_y,yaricap);

    //-------------------------------------------------------------------------------------------------------
    //-------------------------------------CIZIM  KISMI-------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    //CEMBERI CIZDIRME
    initwindow(1280,720);
    int hata;
    hata=graphresult();
    if(hata!=0)
    {
        printf("Grafik moduna geçilmedi.");
        return 0;

    }
    merkez_x+=640;
    merkez_y=(360-merkez_y);
    //KORDINAT DUZLEMINE CIZGI CIZDIRME
    char *cizgi_dik="|";
    char *cizgi_yatay="---";
    char *kordinat_numara;
    setcolor(LIGHTBLUE);
    int P;
    for(P=-2; P<1280; P+=40)
    {
        outtextxy(P,350,cizgi_dik);
    }
    for(P=-9; P<720; P+=40)
    {
        outtextxy(635,P,cizgi_yatay);
    }
    setcolor(LIGHTBLUE);
    // X KORDINATINDAKI DEGERLERI YAZAR
    char kordinat[5];
    int m;
    for(m=-640; m<621; m+=40)
    {
        sprintf(kordinat,"%d",m);
        outtextxy(m+640,365,kordinat);

    }
    //Y KORDINATINTDAKI DEGERLERI YAZAR
    for(m=640; m>-641; m-=40)
    {
        sprintf(kordinat,"%d",m);
        outtextxy(650,360-m,kordinat);
    }
    setcolor(WHITE);
    line(getmaxx()/2,0,getmaxx()/2,getmaxy());//y ekseni
    line(0,ceil(getmaxy()/2),getmaxx(),ceil(getmaxy()/2));//x ekseni
    setcolor(10);
    circle(merkez_x,merkez_y,yaricap);

    //CEMBERIN YARICAPINI CIZER
    setcolor(LIGHTMAGENTA);
    line(merkez_x,merkez_y,getmaxx()/2+en_uzak_x1,getmaxy()/2-en_uzak_y1);

    //NOKTALARI CIZDIRME
    int h;
    char noktalarin_kordinati[10];
    for(h=0; h<nokta_adeti; h++)
    {
        sprintf(noktalarin_kordinati,"(%d,%d)",noktalar[h].x,noktalar[h].y);
        setcolor(15);
        circle((noktalar[h].x+640),(360-noktalar[h].y),3);
        settextstyle(SMALL_FONT,0,5);
        outtextxy(noktalar[h].x+640,(360-noktalar[h].y),noktalarin_kordinati);
    }
    setcolor(LIGHTRED);
    circle(merkez_x,merkez_y,3);
    settextstyle(SMALL_FONT,0,5);
    sprintf(noktalarin_kordinati,"M(%d,%d)",(int)merkez_x-640,360-(int)merkez_y);
    outtextxy(merkez_x,merkez_y,noktalarin_kordinati);

    /*B SPLINE CIZDIRME*/
    /*T DEGERINI KUCUK ARALIKLAR ILE YOLLAYARAK GERI DONEN X VE Y DEGERLERI ARASINDA KISA CIZGILAR CEKEREK DEVAM EDER*/
    float t;
    float eskix=noktalar[0].x;
    float eskiy=noktalar[0].y;
    float geridonenx[1];
    float geridoneny[1];
    for(t=0; t<nokta_adeti-1; t+=0.0005)
    {
        int kosul=1;
        spline_icin_nokta_al(t,noktalar,geridonenx,geridoneny,nokta_adeti,kosul);
        float tx=geridonenx[0];
        float ty=geridoneny[0];
        //printf("%.2f ,%.2f ,%.2f ,%.2f\n",eskix,eskiy,tx,ty);
        line(eskix+640,360-eskiy,640+tx,360-ty);
        eskix=tx;
        eskiy=ty;
    }
    getch();
    closegraph();
    //MALLOC ILE ALINAN BELLEKLER BIRAKILIR
    free(noktalar);
    free(dosyadizgi);
    //-----------------------------------------------------------------------------------------------------------
    //--------------------------------------CIZIM KISMI BITIS----------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------
    return 0;
}
void noktalari_matrisleme(char dosyadizgi[],Nokta *noktalar)
{
    /*DIZGIDEN OKUNAN DOSYALARI INT TÜRÜNDEN BIR DIZIYE ALARAK ISLEM KOLAYLIGI SAGLAMAYI AMAÇLADIK
    BURADAKI IÞLEMLER ILE DIZGIDEN ALINANLARI NOKTALARA BIR MATRISE ATADIK BU SEKILDE NOKTALARI ICEREN BIR MATRIS OLUSTURDUK
    HER (X,Y) DUZLEMINDEKI Y'YI ATAMA ISLEMINDEN SONRA MATRISIN ILK DEGISKENINI BIR ARTTIRIP BIR ALT SATIRA GECISI SAGLADIK
    */
    int i,j=0;
    for(i=0; i<strlen(dosyadizgi); i++)
    {
        if(isdigit(dosyadizgi[i]))
        {

            if(dosyadizgi[i-1]=='{')
            {
                noktalar[j].x=atoi(dosyadizgi+i);

            }
            else if(dosyadizgi[i-2]=='{')
            {
                if(dosyadizgi[i-1]=='-')
                {
                    noktalar[j].x=atoi(dosyadizgi+i)*(-1);
                }
            }
            else if(dosyadizgi[i-1]==',' )
            {
                noktalar[j].y=atoi(dosyadizgi+i);
                j++;
            }
            else if(dosyadizgi[i-2]==',')
            {
                if(dosyadizgi[i-1]=='-')
                {
                    noktalar[j].y=atoi(dosyadizgi+i)*(-1);
                    j++;
                }
            }
        }
    }
}
float iki_nokta_uzakligi(int x1,int y1,int x2,int y2)
{
    /*FONKISYONA GELEN NOKTALAR ARASI UZAKLIGI HESAPLAYIP DONDERIR*/
    int x_kare=pow(x1-x2,2);
    int y_kare=pow(y1-y2,2);
    float sonuc=sqrt(x_kare+y_kare);
    return sonuc;
}
int karakter_sayici(FILE *dosyaptr)
{
    //TXT ICINDEKI KARAKTER SAYISINI HESAPLAYIP DONDERIR
    dosyaptr=fopen("points.txt","r");
    int karakter_sayisi=0, satir_sayisi=0;
    char ch;

    while(feof(dosyaptr)==NULL)
    {
        ch=getc(dosyaptr);
        if(ch=='\n')
        {
            satir_sayisi++;
        }
        karakter_sayisi++;
    }
    return karakter_sayisi;
}
int cember_duzenleme(int x1,int y1,int x2,int y2,int x3,int y3,float *geridonderen_dizi)
{
    float a,b,c;
    a=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
    b=sqrt(pow(x2-x3,2)+pow(y2-y3,2));
    c=sqrt(pow(x3-x1,2)+pow(y3-y1,2));
    float u_2=a+b+c;
    float u=u_2/2;
    float alan=(u*(u-a)*(u-b)*(u-c));
    alan=sqrt(alan);
    float cap=(a*b*c)/(2*alan);
    float yaricap=cap/2;
    float D=2*((x3*(y1-y2))+(x1*(y2-y3))+(x2*(y3-y1)));
    float x=((pow(y3,2)+pow(x3,2))*(y1-y2))+((pow(y1,2)+pow(x1,2))*(y2-y3))+((pow(y2,2)+pow(x2,2))*(y3-y1));
    x=x/D;
    float y=((pow(y3,2)+pow(x3,2))*(x2-x1))+((pow(y1,2)+pow(x1,2))*(x3-x2))+((pow(y2,2)+pow(x2,2))*(x1-x3));
    y=y/D;
    geridonderen_dizi[0]=x;
    geridonderen_dizi[1]=y;
    geridonderen_dizi[2]=yaricap;
    return 1;
}
float spline_icin_nokta_al(float t,Nokta *noktalar,float geridonenx[1],float geridoneny[1],int nokta_adeti,int kosul=0)
{
    /*SPLINE CIZMEK ICIN GEREKLI HESAPLARI YAPARAK KUCUK ARALIKLARLA X VE Y DEGERI DONDERIR O KUCUK ARALIKLAR ARASINDA
    KISA CIZGILER CEKEREK DEVAM EDERIZ*/
    int n0,n1,n2,n3;
    if(!kosul)
    {
        n1=(int)t+1;
        n2=n1+1;
        n3=n2+1;
        n0=n1-1;
    }
    else
    {
        n1 = (int)t;
        n2 = (n1 + 1) % nokta_adeti;
        n3 = (n2 + 1) % nokta_adeti ;
        if(n1>=1)
            n0=n1-1;
        else
            n0=nokta_adeti-1;
    }
    t=t-(int)t;
    float tt=t*t;
    float ttt=tt*t;
    float katsayi1= ttt*(-1)+2.0*tt-t;
    float katsayi2=3.0*ttt-5.0*tt+2.0;
    float katsayi3=ttt*(-3)+4.0*tt+t;
    float katsayi4=ttt-tt;
    float tx=(noktalar[n0].x*katsayi1+noktalar[n1].x*katsayi2+noktalar[n2].x*katsayi3+noktalar[n3].x*katsayi4)/2;
    float ty=(noktalar[n0].y*katsayi1+noktalar[n1].y*katsayi2+noktalar[n2].y*katsayi3+noktalar[n3].y*katsayi4)/2;
    geridonenx[0]=tx;
    geridoneny[0]=ty;
}
