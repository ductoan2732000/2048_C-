/*TRAN DUC TOAN vs LE MIN TUAN with "game2048 kinh dien"*/


#include <Windows.h>
#include <string>
#include <cmath>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <bits/stdc++.h>  // khai bao thu vien
using namespace std ;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 500; // khai bao man hinh win


class LTexture // class bao gom cac ham can thiet trong man hinh render
{
	public:
		LTexture(); // ham khoi tao

		~LTexture(); // ham free

		bool loadFromFile( std::string path ); // ham load texture theo duong dan path

		void free(); //free

		void render( int x, int y, SDL_Rect* clip = NULL ); //ham lay toa do cac anh

		int getWidth(); //chieu dai anh
		int getHeight(); //chieu rong anh

	private: //khai bao bien hang
		SDL_Texture* mTexture;

		int mWidth;
		int mHeight;
};
class load_font // class load font diem
{
	public:
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor ); // load van ban theo string vs mau lua chon
		void copy_font( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
		// ham copy len man hinh ket xuat vs toa do x,y
		int getchieudai()//
		{
		    return chieu_dai_van_ban;//
		}
		int getchieurong()
		{
		    return chieu_rong_van_ban;//
		}
		private:
		SDL_Texture*  vanban = NULL;//
		int chieu_dai_van_ban =0 ;
		int chieu_rong_van_ban=0;// khai bao mac dinh
};

bool init(); //khoi dong sdl va tao cua so


bool loadMedia(); // load


void close(); // free cac ham khi ket thuc chuwng trinh


SDL_Window* gWindow = NULL; // khai bao cua so win

SDL_Renderer* gRenderer = NULL; // khai bao man hinh ket xuat
SDL_Texture * gbackground= NULL; // khai bao tex_ background
SDL_Texture * gwin=NULL; // tex win
SDL_Texture * glose =NULL; // tex lose
Mix_Music * gmusic=NULL; // khai bao bien choi nhac
TTF_Font *gFont = NULL;
SDL_Rect gSpriteClips[ 12 ]; // khai bao bien anh
LTexture gSpriteSheetTexture; // name of class


LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile( std::string path ) // load texture theo duong dan path
{
	free();

	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//chọn mau anh
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free() // ham giai phong
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::render( int x, int y, SDL_Rect* clip ) // tao chieu dai va chieu rong cho anh va copy len man hinh ket xuat
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}
load_font class_load_font; // khai bao ten class
bool load_font::loadFromRenderedText( std::string textureText, SDL_Color textColor )  //load font theo duong dan textureText
{
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
		//Create texture from surface pixels
         vanban= SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if(  vanban== NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			chieu_dai_van_ban = textSurface->w;
			chieu_rong_van_ban = textSurface->h;
		}
		SDL_FreeSurface( textSurface );
	}

	//Return success
	return  vanban!= NULL;
}


void load_font::copy_font( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip  ) // dua van ban len man hinh
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, chieu_dai_van_ban, chieu_rong_van_ban };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer,  vanban, clip, &renderQuad, angle, center, flip );
}
bool init() // ham khoi tao man hinh win vs RENDERER
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
        {
            return false;
					success = false;
        }
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "game 2048", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia() // load cac anh tu 2 den 2048
{
	//Loading success flag
	bool success = true;

	if( !gSpriteSheetTexture.loadFromFile( "anh_tong_hop.png" ) ) // load anh tu anhtonghop
	{
		printf( "Failed to load sprite sheet texture!\n" );
		success = false;
	}
	else
	{
		//Set top left sprite
		for (int i=0;i<3;i++)
        {
            gSpriteClips[i].x =70*i;
            gSpriteClips[i].y =0;
            gSpriteClips [i].w =70;
            gSpriteClips[i].h=70;
        }
        for (int i=3;i<6;i++)
        {
            gSpriteClips[i].x =70*(i-3);
            gSpriteClips[i].y =70;
            gSpriteClips [i].w =70;
            gSpriteClips[i].h=70;
        }
        for (int i=6;i<9;i++)
        {
            gSpriteClips[i].x =70*(i-6);
            gSpriteClips[i].y =70*2;
            gSpriteClips [i].w =70;
            gSpriteClips[i].h=70;
        }
        for (int i=9;i<12;i++)
        {
            gSpriteClips[i].x =70*(i-9);
            gSpriteClips[i].y =70*3;
            gSpriteClips [i].w =70;
            gSpriteClips[i].h=70;
        }
	}

	return success;
}
void Load_all_image(int v[][4]) // load toan bo anh tren mang va copy len man hinh
{
    for (int i=0;i<4;i++)
    {
        for (int j=0;j<4;j++)
        {
            if (v[i][j]==2)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 0 ]) ;
            else if (v[i][j]==4)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 1 ] );
            else if (v[i][j]==8)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 2 ] );
            else if (v[i][j]==16)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 3 ] );
            else if (v[i][j]==32)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 4 ] );
            else if (v[i][j]==64)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 5 ] );
            else if (v[i][j]==128)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 6 ] );
            else if (v[i][j]==256)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 7 ] );
            else if (v[i][j]==512)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 8 ] );
            else if (v[i][j]==1024)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 9 ] );
            else if (v[i][j]==2048)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 10 ] );
            else if (v[i][j]==0)
            gSpriteSheetTexture.render( 435+ j*78, 148+i*78, &gSpriteClips[ 11 ] );
        }
    }
}
void close() // ham giai phong cac bien khi dong ct
{
	//Free loaded images
	gSpriteSheetTexture.free();

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture( std::string path ) // load tex theo duong dan path
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}
void sonn1(int a[][4] ,int doc , int ngang) // ham tao 1 so ngau nhien tren mang
{
	while (true) // vong lap vo han den khi tim dc a.doc_ngang=0
	{
	    doc=(rand()%4), ngang=(rand()%4);
		if (a[doc][ngang]==0)
		{
			a[doc][ngang]=2;
			break;
		}
	}
}
bool dieukien(int &a , int &b) // dieu kien de xap xep la 2 so canh nhau : 1 so =0 va 1 so #=
{
	return a!=0 && b==0;

}

void chuyendoi(int v[]) //ham xap xep don all anh ve 1 ben : vd xap xep ben trai vs 0 0 1 2 -> 1 2 0 0
{
	sort (v,v+4,dieukien);
}
string num_string(int n) // ham dua so ve string
{
	if (n==0) return "0";
    char a[1000000];
    int dem=0;
    for (int i=n;i>0;i=i/10)
    {
        if (i<10)
        {
            a[dem]=i+48;
            break;
        }
        a[dem]=i%10+48;
        dem++;
    }
    a[dem+1]= NULL;
    string b=a;
    for (int i=0;i<b.size()/2; i++)
        swap (b[i] , b[b.size()-i-1]);
    return b;
}
void down(int v[][4],int & change, int temp[][4] , int & diem) // down
{
	change=0; // bien change la bien kiem tra xem sau khi ham down thuc hien xong thi mang ban dau co thay doi hay ko

	for (int i=0;i<4;i++)
	for (int j=0;j<4;j++)
	temp[i][j]=v[i][j]; // dua toan bo mang vao mang khac . mang nay co tac dung kiem tra thay doi sau khi ham down ket thuc


	int downtemp[4];
	for (int q=0;q<4;q++)
	{
		for (int i=3;i>=0;i--)
		downtemp[4-1-i]= v[i][q];
		chuyendoi(downtemp);
		for (int i=3;i>=0;i--)
		v[i][q]=downtemp[4-1-i];
	} // thuc hien dồn số trong     mang downtemp sau do dua tro lai mang ban dau
	for (int i=0;i<4;i++)
	{
		for (int j=3;j>=1;j--)
		{
			if (v[j][i] == v[j-1][i] && v[j][i] !=0)
			{
				v[j][i]*=2;
				diem+=v[j][i];
				v[j-1][i]=0;

			}
		}
	} // cong diem va dồn số  vd : 2 2 0 0 -> 4 0 0 0
	for (int q=0;q<4;q++)
	{
		for (int i=3;i>=0;i--)
		downtemp[4-1-i]= v[i][q];
		chuyendoi(downtemp);
		for (int i=3;i>=0;i--)
		v[i][q]=downtemp[4-1-i];
	} // thực hiện dồn số 1 lần nữa
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
		{
			if (v[i][j] != temp[i][j]) change=1;
		}
	} // nếu sau khi down thay đổi thì change =1 để tiếp tục sinh số ngẫu nhiên và chơi tiếp
}
void up(int v[][4],int & change, int temp[][4], int & diem) // up
{
	change=0;// bien change la bien kiem tra xem sau khi ham up thuc hien xong thi mang ban dau co thay doi hay ko
	for (int i=0;i<4;i++)
	for (int j=0;j<4;j++)
	temp[i][j]=v[i][j];


	int uptemp[4];
	for (int q=0;q<4;q++)
	{
		for (int i=0;i<4;i++)
		uptemp[i]= v[i][q];
		chuyendoi(uptemp);
		for (int i=0;i<4;i++)
		v[i][q]=uptemp[i];
	} // thuc hien dồn số
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<3;j++)
		{
			if (v[j][i] == v[j+1][i])
			{
				v[j][i] *=2;
				diem+= v[j][i];
				v[j+1][i]=0;

			}
		}
	} // cộng điểm và dồn số vd : 2 0 2 0 ->4 0 0 0 và điểm tăng lên 4
	for (int q=0;q<4;q++)
	{
		for (int i=0;i<4;i++)
		uptemp[i]= v[i][q];
		chuyendoi(uptemp);
		for (int i=0;i<4;i++)
		v[i][q]=uptemp[i];
	}
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
		{
			if (v[i][j] != temp[i][j]) change=1;
		}
	} // thực hiện dồn số 1 lần nữa
}

void right(int v[][4],int & change, int temp[][4] ,int &diem)// right
{
	change=0;
	for (int i=0;i<4;i++)
	for (int j=0;j<4;j++)
	temp[i][j]=v[i][j];
	int righttemp[4];
	for (int q=0;q<4;q++)
	{
		for (int i=3;i>=0;i--)
		righttemp[4-1-i]= v[q][i];
		chuyendoi(righttemp);
		for (int i=3;i>=0;i--)
		v[q][i]=righttemp[4-1-i];
	}
	for (int i=0;i<4;i++)
	{
		for (int j=3;j>=1;j--)
		{
			if (v[i][j] == v[i][j-1] && v[i][j] != 0)
			{
				v[i][j]*=2;
				diem+=v[i][j];
				v[i][j-1] =0;

			}
		}
	}
	for (int q=0;q<4;q++)
	{
		for (int i=3;i>=0;i--)
		righttemp[4-1-i]= v[q][i];
		chuyendoi(righttemp);
		for (int i=3;i>=0;i--)
		v[q][i]=righttemp[4-1-i];
	}
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
		{
			if (v[i][j] != temp[i][j]) change=1;
		}
	}
}
void left(int v[][4],int & change, int temp[][4] , int & diem) // left
{
	change=0;
	for (int i=0;i<4;i++)
	for (int j=0;j<4;j++)
	temp[i][j]=v[i][j];


	int lefttemp[4];
	for (int q=0;q<4;q++)
	{
		for (int i=0;i<4;i++)
		lefttemp[i]= v[q][i];
		chuyendoi(lefttemp);
		for (int i=0;i<4;i++)
		v[q][i]=lefttemp[i];
	}
	for (int j=0;j<4;j++)
	{
		for (int k=0;k<3;k++)
		{
			if (v[j][k]==v[j][k+1] && v[j][k] !=0)
			{
				v[j][k]*=2;
				diem+=v[j][k];
				v[j][k+1]=0;
			}
		}
	}
	for (int q=0;q<4;q++)
	{
		for (int i=0;i<4;i++)
		lefttemp[i]= v[q][i];
		chuyendoi(lefttemp);
		for (int i=0;i<4;i++)
		v[q][i]=lefttemp[i];
	}
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
		{
			if (v[i][j] != temp[i][j]) change=1;
		}
	}
}
bool tieptuc_or_end(int arr [][4] ) //kiểm tra xem trò chơi có thế tiếp tục hay "you lose"
{
	int count1 = 0, count2 = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			if (arr[i][j] != 0)
				count1++;
	}
	if (count1 != 16)
		return true;
	for (int i = 0; i < 4; i++) {
		if (arr[i][0] == arr[i][1] || arr[i][1] == arr[i][2] || arr[i][2] == arr[i][3])
			return true;
	}
	for (int i = 0; i < 4; i++) {
		if (arr[0][i] == arr[1][i] || arr[1][i] == arr[2][i] || arr[2][i] == arr[3][i])
			return true;
	}
	return false;
}
bool win_or_con(int n[][4]) // ham kiem tra win : nếu 1 số trong mảng là 2048 thì trò chơi kết thúc và bạn thắng
{
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        if (n[i][j]==2048)
        return true;
    return false;
}
int main( int argc, char* args[] )
{
	srand(time(0)); // su dung ham nay de cho ra cac so ngau nhien ko theo quy lust
	int v[4][4], change=0, temp[4][4], lost=0, win=0 , diem=0; // khoi tao mang 4*4 , bien kiem tra
	// mảng v là mảng chính , change là biến kiểm tra xem mỗi lần ấn "lên,xuống,trái,phải" thì mảng v có thay đổi hay ko
	//temp là mảng tạm có tác dụng hỗ trợ để kiểm tra change mà thôi
	//lost là biến kiểm tra bạn thua
	// win là biến kiểm tra bạn thắng,
	// điểm là biến chứa điểm hiện tại


	string dieminra; // diem cua ban sau khi chuyển đổi từ int diem  -> string
	for (int i=0;i<4;i++) // khoi tao mang 4*4 co cac phan tu =0
    {
        for (int j=0;j<4;j++)
            v[i][j]=0;
    }

    sonn1(v,0,0);
    sonn1(v,0,0);  // dua 2 so ngau nien vao 2 vi tri trong mang 4*4
	if( !init() )  // kiem tra khoi tao
	{
		printf( "Failed to initialize!\n" );
		return 0;
	}

		//Load media
    if( !loadMedia() ) // kiem tra load medis
    {
        printf( "Failed to load media!\n" );
        return 0;
    }
    gbackground= loadTexture("background.png"); // load background =tex
    SDL_RenderCopy( gRenderer, gbackground, NULL, NULL );
    Load_all_image(v); // load cac so tren mang len man hinh

    gFont = TTF_OpenFont( "impact.ttf", 35 ); // chon font va kich co chu
    SDL_Color textColor = { 0, 0, 0 }; // chon mau font
    if( !class_load_font.loadFromRenderedText( num_string(diem) , textColor ) ) // kiem tra load diem
    {
        printf( "Failed to render text texture!\n" );
        return  0;
    }
		class_load_font.copy_font( 247,144);

        gmusic= Mix_LoadMUS( "nhac_game_2048.mp3"); // load music
        Mix_PlayMusic( gmusic, -1 ); // choi nhac voi che do phat lai

        bool quit = false; //Event handler

        SDL_Event e;//While application is running
        // trong su kien dien ra : neu ko co dau hieu dung ban phim thi giu nguyen man hinh ,
        // con neu co ban phim tac dong thi kiem tra ng dung an phim gi :
        // neu an thoat thi ket thuc chuong trinh con la up,right,down,left thi xoa toanf bo man hinh
        // load lai toan bo anh moi sau khi di chuyen cac so tren man hinh
        while(quit== false)
        {

            while( SDL_PollEvent( &e ) != 0 )
            {
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }

                else if( e.type == SDL_KEYDOWN )
                {
                    switch( e.key.keysym.sym )
                    {
                        case SDLK_UP:
                        {
                            up(v, change , temp, diem);
                            if (change ==1)
                            {
                                SDL_RenderClear (gRenderer);
                                sonn1(v, 0,0);

                                SDL_RenderCopy( gRenderer, gbackground, NULL, NULL );
                                Load_all_image(v); // neu up thi xoa man hinh va in lai background cung nhuw toan bo anh
                                if( !class_load_font.loadFromRenderedText( num_string(diem) , textColor ) )
                                {
                                    printf( "Failed to render text texture!\n" );
                                    return  0;
                                }
                                class_load_font.copy_font( 247,144);
                            }
                            break;
                        }



                        case SDLK_DOWN:
                        {

                            down(v, change , temp, diem);
                                if (change ==1)
                                {
                                    SDL_RenderClear (gRenderer);
                                    sonn1(v, 0,0);

                                    SDL_RenderCopy( gRenderer, gbackground, NULL, NULL );
                                    Load_all_image(v);
                                    if( !class_load_font.loadFromRenderedText( num_string(diem) , textColor ) )
                                {
                                    printf( "Failed to render text texture!\n" );
                                    return  0;
                                }
                                class_load_font.copy_font( 247,144);
                                }

                                break;
                            }


							case SDLK_LEFT:

							{
							      left(v, change , temp, diem);
							       if (change ==1)
                                   {
                                       SDL_RenderClear (gRenderer);
                                       sonn1(v, 0,0);

                                        SDL_RenderCopy( gRenderer, gbackground, NULL, NULL );
                                       Load_all_image(v);
                                        if( !class_load_font.loadFromRenderedText( num_string(diem) , textColor ) )
                                {
                                    printf( "Failed to render text texture!\n" );
                                    return  0;
                                }
                                class_load_font.copy_font( 247,144);
                                   }
							        break;
							    }

							case SDLK_RIGHT:
							{
							        right(v, change , temp, diem);
							       if (change ==1)
                                   {
                                       SDL_RenderClear (gRenderer);
                                       sonn1(v, 0,0);

                                        SDL_RenderCopy( gRenderer, gbackground, NULL, NULL );
                                       Load_all_image(v);
                                        if( !class_load_font.loadFromRenderedText( num_string(diem) , textColor ) )
                                {
                                    printf( "Failed to render text texture!\n" );
                                    return  0;
                                }
                                class_load_font.copy_font( 247,144);

                                   }
							        break;
							    }


						}

						if (tieptuc_or_end(v)== false) {quit=true;lost=1;} // kiem tra tiep tuc
						if (win_or_con(v)==true) {quit=true;win=1;} //kiem tra chien thang

					}
				}

				SDL_RenderPresent(gRenderer); // load lai anh len man hinh
			}

if (lost==1) // neu ban thua load anh lose sau 5s thi dong ct
{
    SDL_RenderClear (gRenderer);
    glose =loadTexture("you_lose.png");
    SDL_RenderCopy( gRenderer, glose, NULL, NULL );
    SDL_RenderPresent(gRenderer);
    SDL_Delay(5000);
}
else if (win==1)  // neu ban thang load anh win sau 5s thi dong ct
{
    SDL_RenderClear (gRenderer);
    glose =loadTexture("you_win.png");
    SDL_RenderCopy( gRenderer, gwin, NULL, NULL );
    SDL_RenderPresent(gRenderer);
    SDL_Delay(5000);
}


	close();//Free resources and close SDL

	return 0;
}

