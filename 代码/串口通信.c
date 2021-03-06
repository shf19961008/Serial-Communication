#include <reg52.h>
#define uchar unsigned char
void init_com(){
	SCON=0X50;			//设置串口的工作方式为1
	TMOD=0X20;			//设置计数器工作方式为2
	TH1=0Xfd;		    //计数器初始值设置，波特率是9600
	TL1=0Xfd;
	TR1=1;					    //打开计数器
	EA=1;        //总中断允许位
	ES=1;        //串行口中断允许控制位	
}


void send_data(uchar *str){
	uchar i;
	for(i=0;str[i]!='\0';i++){
		SBUF = str[i];
		while(!TI);
		TI = 0;
	}
}

void delay_ms(uchar ms)        		         //延时程序,延时约ms毫秒
{    
	uchar i,j;
	while(ms--)
		for(i = 0 ;i<5; i++)
			for(j=0;j<220;j++);
}



sbit DS = P2^5;                            //将单片机的P2.5引脚定义为74HC595的串行数据输入端
sbit STCP = P2^6;                          //将单片机的P2.6引脚定义为74HC595的存储寄存器时钟输入端
sbit SHCP = P2^7;                          //将单片机的P2.7引脚定义为74HC595的移位寄存器时钟输入端

sbit wx1=P0^4;                                            //位选0，低电平有效，选中第一个数码管
sbit wx2=P0^5;                                            //位选1，低电平有效，选中第二个数码管
sbit wx3=P0^6;                                            //位选2，低电平有效，选中第三个数码管
sbit wx4=P0^7;                                            //位选3，低电平有效，选中第四个数码管
/*共阴极数码管显示0~9时的输入数据*/
uchar table[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
/*数码管显示函数，利用74HC595实现串行输入并行输出*/

uchar display_data[4];
uchar received_data[4];
uchar cnt=0;
void ser_inout(uchar datas){
	uchar i;
	STCP = 0;	                              
	for(i=0 ; i < 8 ; i++)                   //从高位到低位，依次传送数据
	{
		SHCP = 0;                              
		DS=datas & 0x80;		
		datas<<=1;                            
		SHCP = 1;                              //P2.7引脚置1，产生上升沿，将P2.5引脚上的数据传入74HC595
	}
	STCP = 1;                                //P2.6置1，产生上升沿
}


void display(){
	while(1){//一直刷新显示
		wx1=0;
		ser_inout(display_data[0]);
		delay_ms(1);
		wx1=1;
		
		wx2=0;
		ser_inout(display_data[1]);
		delay_ms(1);
		wx2=1;
		
		wx3=0;
		ser_inout(display_data[2]);
		delay_ms(1);
		wx3=1;
		
		wx4=0;
		ser_inout(display_data[3]);
		delay_ms(1);
		wx4=1;
	}
}

void update_display() interrupt 4{//串口中断号4，有串口中断时自动调用本函数。
	uchar i;
	//接收数据
	if(RI==1){
		received_data[cnt%4]=SBUF;
		cnt++;
		RI=0;
	}
	//更新显示数据
	if(cnt%4==0){
		for(i=0;i<4;i++) display_data[i]=table[received_data[i]-'0'];
	}
}

int main(){
    init_com();//初始化串行口设置
	send_data("Hello World!");
	display();//一直显示并等待中断更新显示
	return 0;
}