#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int add_item(int data, int start_time , int current_state,int window_size,int drop_array[],int size); 	/*add data to queue*/
void delete_item(int rtt);	 									/*delete data from queue*/
int queue_is_full(int MAXSIZE);										 /*controled of queue full or not*/
void state_control(); 											/*all of the state of data is increment 1*/
int process_is_finish(int rtt, int timeout,int packet_size,int number_of_packets,int last_packet_size); /*process finish control*/
int drop_index_control(int drop_array[] ,int array_size, int number); 					/*search drop index for adding data*/
print_time(int time); 
void print();
int queue_is_empty();
int number=0;
struct node *front;
struct node *rear;
float current_data_size=0;


typedef struct node
{
    int data;
    struct node* next;
    int start_time;
    int current_state;
    int drop;
    int recieve;
    int timeout;
};

int main(int argc ,char*argv[])
{
    int window_size=atoi(argv[1]);
    float data_size=atof(argv[2]);
    float packet_size=atof(argv[3]);
    int timeout=atoi(argv[4]);
    int rtt=atoi(argv[5]);
    FILE* fp;
    char line[250];
    int drop_index_array[500];
    int i=0;
    int ch;
    float last_packet_size;
    int array[500];
    fp=fopen(argv[6],"r");

    while(!feof(fp))
    {
        fgets(line, 100, fp);
        drop_index_array[i++]=atoi(line);
    }
    fclose(fp);

    int number_of_packets;
    if((int)data_size % (int)packet_size==0) /*number of packets calculated*/
    {
        number_of_packets=data_size/packet_size;
        last_packet_size=data_size;     /*last packet size is calculated*/
    }

    else
    {
        number_of_packets=(data_size/packet_size)+1; 
        last_packet_size=data_size-(packet_size*(number_of_packets-1)); 
    }
    int flag;
    int control=0;
    int status;
    int finish_control;
    int len=0;
    int z=0;

    while(drop_index_array[z++]!=NULL)   /*drop index length is calculated*/
        len++;

    int time=1;
    int finish_packet=0;
    printf("<SIMULATION TIME> (00:00:00)\n");
    printf("<EVENTS>\n");
    printf("- Data size sent so far is 0.00 Byte\n");
    printf("<WINDOW STATE>: empty\n\n");

    while(control!=number_of_packets || finish_packet!=-1)
    {
        if(queue_is_empty()!=-1 && time!=1)    /*finish or not control*/
        print_time(time);
        else if(time==1)
            print_time(time);

        if(time==1)
        {
            flag=add_item(control,time,0,window_size,drop_index_array,len);   /*if flag is equals 0 , data is adding*/
            process_is_finish(rtt,timeout,packet_size,number_of_packets,last_packet_size);
        }


        else if(finish_packet==0)
        {
            finish_control=process_is_finish(rtt,timeout,packet_size,number_of_packets,last_packet_size);  /*if finish control is 1 data is tranferred*/
            if(finish_control==1)
            {
                delete_item(rtt);
                if(control!=number_of_packets)
                {
                    flag=add_item(control,time,0,window_size,drop_index_array,len);
                }
            }
            else
            {
                flag=add_item(control,time,0,window_size,drop_index_array,len);
            }
        }
        else
        {
            if(queue_is_empty()!=-1)
            {
                finish_control=process_is_finish(rtt,timeout,packet_size,number_of_packets,last_packet_size);

                if(finish_control==1)
                {
                    delete_item(rtt);
                }
            }
            else
                finish_packet=-1;  /*finish packet*/
        }

        if(flag==0 && control!=number_of_packets)
        {
            control++;
        }
        else if(control==number_of_packets && finish_packet!=-1)
            finish_packet=1;

        if(finish_packet!=-1)
        {
            printf("- Data size sent so far is %4.2f Byte\n",current_data_size);
            state_control();
            print();
            time++;
        }
    }


    int number_of_drop=0;
    int t=0;
    while(number_of_packets>drop_index_array[t++])
        number_of_drop++;

    float total_average=0.0;
    total_average=((float)(number_of_packets*rtt)+(number_of_drop*timeout))/number_of_packets;   /*avrage calculated*/

    printf("***************************************************\n");
    printf("*                 TRANSFER REPORT                 *\n");
    printf("***************************************************\n");
    printf("Parameter Setting:\n");
    printf("---------------------------------------------------\n");
    printf("Window Size                          :           %02d\n",window_size);
    printf("Timeout                              :    %04d Sec.\n",timeout);
    printf("RTT                                  :    %04d Sec.\n",rtt);
    printf("Data Size                            : %4.2f Byte\n",data_size);
    printf("Packet Size                          :   %4.2f Byte\n",packet_size);
    printf("---------------------------------------------------\n");
    printf("Results:\n");
    printf("---------------------------------------------------\n");
    printf("Number of packet to send the data    :         %d\n",number_of_packets);
    printf("Number of packet dropped             :         %04d\n",number_of_drop);
    printf("Average time to send a single packet :  %3.3f Sec.\n",total_average);
    printf("***************************************************\n");

    return 0;
}

int add_item(int data, int start_time , int current_state,int window_size,int drop_array[],int size)
{
    int control=queue_is_full(window_size);
    if(control!=-1)
    {
        int drop;
        struct node *temp;
        temp = (struct node*)malloc(sizeof(struct node));
        temp->data=data;
        temp->next = NULL;
        temp->start_time=start_time;
        temp->current_state=current_state;
        drop=drop_index_control(drop_array , size, data);
        temp->drop=drop;
        temp->recieve=0;
        temp->timeout=0;
        if (rear  ==  NULL)  /*    queue is empty*/
        {
            front =temp;
            rear = temp;
        }
        else
        {
            rear->next = temp;
            rear = temp;
        }
        number++;
        printf("- A new data packet (id:%d) has now been sent!\n",temp->data);
    }
    return control;
}

void delete_item(int rtt)
{
    struct node *temp;

    temp = front;
    if (front == NULL) /*queue is empty*/
    {
        front =NULL;
        rear = NULL;
    }
    else
    {
        while(temp!=NULL)
        {
            if(front->current_state==rtt && front->drop==0)   /*delete data control*/
            {

                front=front->next;
                number--;
            }
            temp=temp->next;
        }
    }
    free(temp);
    if(queue_is_empty()==-1)
    {
        rear=NULL;
        front=NULL;
    }
}

int queue_is_full(int MAXSIZE)
{
    int result=0;
    if(number==MAXSIZE)  /* queue is full*/
        result=-1;

    return result;

}

int drop_index_control(int drop_array[] ,int array_size, int number)
{
    int i,result=0;
    for(i=0; i<array_size; i++)
    {
        if(number==drop_array[i])
            result++;
    }
    return result;

}

void state_control()
{
    struct node *temp;
    temp=front;
    while(temp!=NULL)
    {
        temp->current_state++;  /*state is increment of all time*/
        temp=temp->next;
    }
}

int process_is_finish(int rtt, int timeout,int packet_size,int number_of_packets,int last_packet_size)
{
    int result=0;
    struct node *temp;
    temp=front->next;
    struct node *temp2;
    temp2=front;
    while(temp2!=NULL)
    {
        if(temp2->current_state>=rtt && temp2->drop==0 && temp2->recieve==0)   /*recieve data control*/
        {
            result=1;
            printf("- Data packet (id:%d) has now been ACK'ed by receiver!\n",temp2->data);
            temp2->recieve=1;
            if(temp2->data!=number_of_packets-1)
                current_data_size+=packet_size;
            else
                current_data_size+=last_packet_size;
        }

        else if(temp2->drop!=0  && temp2->current_state==timeout)
        {
            temp2->drop--;
            temp2->current_state = 0;
            temp2->timeout=1;
        }
        if(temp2->timeout==1 && temp2->current_state==0)
            printf("- It is timeout for data packet (id:%d), so it has been resent now!\n",temp2->data);
    temp2=temp2->next;
    }



    while(temp!=NULL)
    {
        if(temp->drop!=0 && temp->current_state==timeout)
        {
            temp->drop--;
            temp->current_state=0;
        }
        temp=temp->next;
    }

    return result;
}

int queue_is_empty()
{
    struct node *temp;
    temp=front;
    int result=0;
    if (temp == NULL) /* queue is empty*/
        result = -1;

    return result;
}

void print()
{
    struct node *temp;
    struct node *temp2;
    temp=front;
    temp2=front;

    while(temp!=NULL)
    {
        if(temp->current_state>=10 && temp->drop==0)
        {
            temp->current_state=10;
        }

        temp=temp->next;
    }
    printf("<WINDOW STATE>: ");
    int control_index=0;
    print_window(front,&control_index);
    if(queue_is_empty()==-1)   /*queue is finish or not*/
        printf("empty");
    printf("\n\n");
}

print_time(int time)    /*calculated time*/
{
    printf("<SIMULATION TIME> ");
    int hr,t,min,sec;
    hr = time/3600;
    t = time%3600;
    min = time/60;
    sec = time%60;
    if(hr>=10)
        printf("(%d",hr);
    else
        printf("(0%d:",hr);

    if(min>=10)
        printf("%d",min);
    else
        printf("0%d:",min);

    if(sec>=10)
        printf("%d)",sec);
    else
        printf("0%d)",sec);

    printf("\n");
    printf("<EVENTS>\n");
}


print_window(struct node *temp,int *index)  /*print queue*/
{
    if(temp!=NULL)
    {

        print_window(temp->next,&index);
        if(*index==0)
        printf("%d ->%d",temp->data,temp->current_state);
        else
            printf("%d ->%d | ",temp->data,temp->current_state);

        *index++;
    }

}