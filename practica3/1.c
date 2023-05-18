El A:

(Si es mesa):

while(!M || !P)
    pthread_cond_wait(&cv,%lk);

if(!M || !p)
    destroy_house();

unlock(lk);

(si es HOARE):

if(!M || !P) // no hace falta un while, porque se preserva la condicion desde el signal hasta el wait.
    pthread_cond_wait(&cv,%lk);

if(!M || !p)
    destroy_house();

unlock(lk);
