
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define max_boyut 7

int X[max_boyut][max_boyut];
int Y[max_boyut][max_boyut];
int sonuc[max_boyut][max_boyut];
pthread_mutex_t lock;


typedef struct
{
    int satir;
    int sutun;
    int boyut;
} thread_struct;

void *matris_carp(void *arg)
{
    thread_struct *veri = (thread_struct *)arg;
    int satir = veri->satir;
    int sutun = veri->sutun;
    int boyut = veri->boyut;
    int tempsum = 0;
    pthread_mutex_lock(&lock);//kritik bölgeye giris icin lock kullandım
    for (int i = 0; i < boyut; i++)
    {
        tempsum += X[satir][i] * Y[i][sutun];
    }
    sonuc[satir][sutun] = tempsum;
    pthread_mutex_unlock(&lock);//isim bittikten sonra lock u serbest bıraktım
    free(veri);//isim bittikten sonra ayrılan alani serbest bıraktım
    pthread_exit(NULL);
}

// Dosya okuma fonksiyonu
void DosyadanMatrisOku(const char *dosyaAdi, int matris[max_boyut][max_boyut], int *satir, int *sutun)
{
    FILE *dosya = fopen(dosyaAdi, "r");

    if (!dosya)
    {
        printf("Dosya açılırken hata oluştu.\n");
        exit(-1);
    }

    *satir = 0;
    *sutun = 0;

    // Satır ve sütun sayısını bulma
    char karakter;
    while ((karakter = fgetc(dosya)) != EOF)
    {
        if (karakter == '\n')
        {
            (*satir)++;
        }
        else if (karakter == ' ' && *satir == 0)
        {
            (*sutun)++;
        }
    }

    // Boyut kontrolü
    if (*satir >= max_boyut || *sutun >= max_boyut)
    {
        printf("Matris boyutu 7x7 veya daha küçük olmalıdır.\n");
        fclose(dosya);
        exit(-1);
    }

    fseek(dosya, 0, SEEK_SET);  // Dosyayı tekrar başa alip okumak icin

    // Matrisi dosyadan okuma
    for (int i = 0; i < *satir; i++)
    {
        for (int j = 0; j < *sutun; j++)
        {
            if (fscanf(dosya, "%d", &matris[i][j]) != 1)
            {
                printf("Dosyadan okuma hatası.\n");
                fclose(dosya);
                exit(-1);
            }
        }
    }

    fclose(dosya);
}


// Thread oluşturma ve bekletme fonksiyonu
void ThreadOlusturVeBeklet(int boyut)
{
    pthread_t threads[max_boyut][max_boyut];

    // Thread oluştur
    for (int i = 0; i < boyut; i++)
    {
        for (int j = 0; j < boyut; j++)
        {
            thread_struct *veri = malloc(sizeof(thread_struct)); // Her thread icin dinamik olarak yer ayrildi
            veri->satir = i;
            veri->sutun = j;
            veri->boyut = boyut;
            pthread_create(&threads[i][j], NULL, matris_carp, (void *)veri);
        }
    }

    // Thread beklet
    for (int i = 0; i < boyut; i++)
    {
        for (int j = 0; j < boyut; j++)
        {
            pthread_join(threads[i][j], NULL);
        }
    }
}

// Sonuçları yazdırma fonksiyonu
void Yazdir(int boyut)
{
    for (int i = 0; i < boyut; i++)
    {
        for (int j = 0; j < boyut; j++)
        {
            printf("%d ", sonuc[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    int boyut = max_boyut;

    // Matrisleri dosyalardan okuma
    DosyadanMatrisOku("inputA.txt", X, &boyut, &boyut);
    DosyadanMatrisOku("inputB.txt", Y, &boyut, &boyut);

    pthread_mutex_init(&lock, NULL);// Mutex'i baslattim
    ThreadOlusturVeBeklet(boyut);//fonksiyonumu cagirdim
    pthread_mutex_destroy(&lock);//mutex i serbest biraktim
    Yazdir(boyut);
    return 0;
}
