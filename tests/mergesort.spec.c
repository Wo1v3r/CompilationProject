void mergesort(int a[30],int i,int j)
{
    int mid;
        
    if(i<j)
    {
        mid=(i+j)/2;
        mergesort(a,i,mid);
        mergesort(a,mid+1,j);
        merge(a,i,mid,mid+1,j);
    }
}
 
void merge(int a[30],int i1,int j1,int i2,int j2)
{
    int temp[50];
    int i,j,k;
    i=i1;
    j=i2;
    k=0;
    
    while(i<=j1 && j<=j2)
    {
        if(a[i]<a[j])
        {
          temp[k = k + 1]=a[i = i + 1];
        }
        else {
        temp[k = k + 1]=a[j = j + 1];
        }
    }
    
    while(i<=j1)
        temp[k = k + 1]=a[i = i + 1];
        
    while(j<=j2)
        temp[k = k + 1]=a[j = j + 1];
        

    for(i=i1 ; i<=j2 ; i = i + 1)
        a[i]=temp[j];
}

int main()
{
    int a[30],n,i;

      scanf("%d",&n);

      
      for(i=0;i<n;i= i + 1)
         scanf("%d",a[i]);
          
      mergesort(a,0,n-1);
      

      for(i=0;i<n;i = i + 1)
          printf("%d ",&a[i]);
  
      return 0;
  }
   