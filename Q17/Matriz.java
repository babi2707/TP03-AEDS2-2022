
/***************************************
 * 
 * @author Barbara Luciano Araujo
 * Matricula: 748190
 * TP03 - Questão 17 - Matriz Dinâmica em Java
 * 
****************************************/

// --------------- Celula ---------------
class Celula {

    // ---------- atributos ----------
    public int elemento;
    public Celula inf, sup, esq, dir;
    // -------------------------------

    // ---------- construtores ----------

    public Celula() {
        this(0, null, null, null, null);
    }

    public Celula(int elemento, Celula inf, Celula sup, Celula esq, Celula dir) {
        this.elemento = elemento;
        this.inf = inf;
        this.sup = sup;
        this.esq = esq;
        this.dir = dir;
    }

    // ----------------------------------
}
// --------------------------------------

// --------------- matriz dinamica ---------------
class MatrizD {

    // ---------- atributos ----------
    public Celula inicio;
    public int linha, coluna;
    // -------------------------------

    // ---------- construtores ----------

    public MatrizD() {
        this(3, 3);
    }

    public MatrizD(int linha, int coluna) {
        this.linha = linha;
        this.coluna = coluna;

        this.inicio = new Celula();

        Celula tmp = this.inicio;
        for(int i = 1; i < this.linha; i++) {
            tmp.inf = new Celula();
            tmp.inf.sup = tmp;
            tmp = tmp.inf;
        }
        tmp = null;

        for(int i = 0; i < this.linha; i++) {
            tmp = this.inicio;
            for(int j = 0; j < i; j++) {
                tmp = tmp.inf;
            }

            for(int k = 1; k < this.coluna; k++) {
                tmp.dir = new Celula();
                tmp.dir.esq = tmp;
                tmp = tmp.dir;
            }
        }
        tmp = null;

        if(this.linha > 1) {
            tmp = this.inicio.inf;
            for(int i = 1; i < this.linha; i++) {
                Celula superior = tmp.sup;
                Celula inferior = tmp;

                while(superior != null && inferior != null) {
                    superior.inf = inferior;
                    inferior.sup = superior;

                    superior = superior.dir;
                    inferior = inferior.dir;
                }

                tmp = tmp.inf;
            }
        }
    }

    // ----------------------------------

    // ---------- somar ----------

    private int somar(MatrizD a, MatrizD b, int x, int y) {
        int soma;
        Celula c = a.inicio;
        Celula d = b.inicio;

        for (int i = 0; i < x; i++) {
            c = c.dir;
            d = d.dir;
        }

        for (int i = 0; i < y; i++) {
            c = c.inf;
            d = d.inf;
        }

        soma = c.elemento + d.elemento;

        return soma;
    }

    public MatrizD soma(MatrizD x) {
        MatrizD resp = new MatrizD(x.linha, x.coluna);
        Celula a = resp.inicio;
        for (int i = 0; i < this.linha; i++) {
            Celula b = a;
            for (int j = 0; j < this.coluna; j++) {
                b.elemento = somar(this, x, j, i);

                b = b.dir;
            }
            a = a.inf;
        }

        return resp;
    }
    
    // ---------------------------

    // ---------- multiplicar ----------

    private int multiplicar(MatrizD a, MatrizD b, int x, int y) {
        int multiplicacao = 0;
        Celula c = a.inicio;
        Celula d = b.inicio;

        for (int i = 0; i < x; i++) {
            c = c.inf;
        }

        for (int i = 0; i < y; i++) {
            d = d.dir;
        }

        int e, f;
        while (c != null && d != null) {
            e = c.elemento;
            f = d.elemento;
            multiplicacao += e * f;
            c = c.dir;
            d = d.inf;
        }

        return multiplicacao;
    }

    public MatrizD multiplicacao(MatrizD x) {
        MatrizD resp = new MatrizD(x.linha, x.coluna);
        Celula a = resp.inicio;
        int i = 0;
        while (a != null && i < this.linha) {
            Celula b = a;
            int j = 0;
            while (b != null && j < this.coluna) {
                b.elemento = multiplicar(this, x, i, j);
                b = b.dir;
                j++;
            }
            a = a.inf;
            i++;
        }

        return resp;
    }

    // ---------------------------------

    // ---------- diagonal principal ----------

    public void mostrarDiagonalPrincipal() {
        if (this.linha == this.coluna) {
            Celula i = inicio;
            while (i != null) {
                MyIO.print(i.elemento + " ");

                i = i.dir;
                if (i != null) {
                    i = i.inf;
                }
            }
            MyIO.println("");
        }
    }

    // ----------------------------------------

    // ---------- diagonal secundaria ----------

    public void mostrarDiagonalSecundaria() {
        if (this.linha == this.coluna) {
            Celula i;

            for (i = inicio; i.dir != null; i = i.dir)
                ;

            while (i != null) {
                MyIO.print(i.elemento + " ");
                i = i.inf;
                if (i != null) {
                    i = i.esq;
                }
            }
            MyIO.println("");
        }
    }

    // -----------------------------------------
}
// -----------------------------------------------

public class Matriz {

    public static void main(String args[]) throws Exception {

        MyIO.setCharset("utf-8");

        // ----- inicializa��o das vari�veis -----
        int numOp = MyIO.readInt();
        // ----------------------------------------

        for (int i = 0; i < numOp; i++) {
            int linhas = MyIO.readInt();
            int colunas = MyIO.readInt();

            MatrizD matriz1 = new MatrizD(linhas, colunas);

            for (Celula aux = matriz1.inicio; aux != null; aux = aux.inf) {
                for (Celula aux2 = aux; aux2 != null; aux2 = aux2.dir) {
                    aux2.elemento = MyIO.readInt();
                }
            }

            linhas = MyIO.readInt();
            colunas = MyIO.readInt();
            MatrizD matriz2 = new MatrizD(linhas, colunas);
            for (Celula aux = matriz2.inicio; aux != null; aux = aux.inf) {
                for (Celula aux2 = aux; aux2 != null; aux2 = aux2.dir) {
                    aux2.elemento = MyIO.readInt();
                }
            }

            MatrizD soma = matriz1.soma(matriz2);

            MatrizD multiplicacao = matriz1.multiplicacao(matriz2);

            matriz1.mostrarDiagonalPrincipal();
            matriz1.mostrarDiagonalSecundaria();

            for (Celula aux = soma.inicio; aux != null; aux = aux.inf) {
                for (Celula aux2 = aux; aux2 != null; aux2 = aux2.dir) {
                    MyIO.print(aux2.elemento + " ");
                }
                MyIO.println("");
            }

            for (Celula aux = multiplicacao.inicio; aux != null; aux = aux.inf) {
                for (Celula aux2 = aux; aux2 != null; aux2 = aux2.dir) {
                    MyIO.print(aux2.elemento + " ");
                }
                MyIO.println("");
            }
        }
    }
}