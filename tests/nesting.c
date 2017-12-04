int test(int a){
    string flag = true;
    int grade;
    if(!testOk()){
        for(a=0;a<100 && flag;a=rand())
            grade = a;
            if(grade >= 90){
                flag = false;
            }
    }
    else
        grade = 100;
    return grade;
}