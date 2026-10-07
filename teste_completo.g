principal
{
    a, b : int;
    c, d : car;
}
{
    a = 10;
    b = 20;
    c = 'x';
    d = 'y';

    escreva a;
    escreva c;
    escreva "teste completo";
    novalinha;

    a = b + 5;
    b = a - 2;
    a = b * 3;
    b = a / 2;
    a = -b;

    a = a > b;
    a = a < b;
    a = a >= b;
    a = a <= b;
    a = a == b;
    a = a != b;

    a = a & b;
    a = a || b;
    a = !a;

    leia a;
    leia c;

    se (a > 0) entao
        escreva a;
    fimse

    se (a > 0) entao
        escreva a;
    senao
        escreva b;
    fimse

    enquanto (a > 0)
        {
            x : int;
        }
        {
            x = a;
            a = a - 1;
        }

    {
        x : car;
    }
    {
        x = 'z';
        escreva x;
    }
}
