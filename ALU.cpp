//8 BIT ALU Justė Čiuderytė

#include <iostream>

// ------------------------
// -- LOGINIAI ELEMENTAI --
// ------------------------

bool NAND(bool x, bool y)
{
	if (x == 1 && y == 1)
		return 0;
	else
		return 1;
}

bool OR(bool x, bool y)
{
	bool t1 = NAND(x, x);
	bool t2 = NAND(y, y);
	bool rez = NAND(t1, t2);
	return rez;
}

bool AND(bool x, bool y)
{
	return NAND(NAND(x, y), NAND(x, y));
}

bool NOT(bool x)
{
	return NAND(x, x);
}

bool XOR(bool x, bool y)
{
	bool t1 = NAND(x, y);
	bool t2 = NAND(x, t1);
	bool t3 = NAND(y, t1);
	bool rez = NAND(t2, t3);
	return rez;
}

bool NOR(bool x, bool y)
{
	bool t1 = NAND(x, x);
	bool t2 = NAND(y, y);
	return NAND(NAND(t1, t2), NAND(t1, t2));
}

// ------------------------------------
// ----- HALFADDER,FULLADDER,MUX ------
// ------------------------------------

void HALFADDER(bool A, bool B, bool& sum, bool& carry)
{
	sum = XOR(A, B);
	carry = AND(A, B);
}

void FULLADDER(bool A, bool B, bool CarryIn, bool& sum, bool& CarryOut)
{
	bool sum1, carry1, carry2;
	HALFADDER(A, B, sum1, carry1);
	HALFADDER(CarryIn, sum1, sum, carry2);
	CarryOut = OR(carry1, carry2);

}
bool MUX(bool t3, bool t4, bool t5, bool sum, bool M0, bool M1)
{
	bool k1 = AND(AND(t3, NOT(M0)), NOT(M1));
	bool k2 = AND(AND(t4, M0), NOT(M1));
	bool k3 = AND(AND(t5, NOT(M0)), M1);
	bool k4 = AND(AND(sum, M0), M1);
	bool Output = OR(OR(k1, k2), OR(k3, k4));
	return Output;
}

// ------------------------
// ----- 1 BITO ALU ------
// ------------------------


void ONEBITALU(bool A, bool B, bool ENA, bool ENB, bool M0, bool M1, bool CarryIn, bool& CarryOut, bool& Output)
{
	bool sum;
	bool t1 = AND(A, ENA);
	bool t2 = AND(B, ENB);
	bool t3 = XOR(t1, t2);
	bool t4 = OR(t1, t2);
	bool t5 = NOT(t2);
	FULLADDER(t1, t2, CarryIn, sum, CarryOut);
	Output = MUX(t3, t4, t5, sum, M0, M1);
}

//--------- DECODER ---------

void decoder3to8(bool D[3], bool R[8])
{
		R[0] = AND(AND(NOT(D[0]), NOT(D[1])), NOT(D[3])); //000
		R[1] = AND(AND(NOT(D[0]), NOT(D[1])), D[3]); //001
		R[2] = AND(AND(NOT(D[0]), NOT(D[2])), D[1]); //010
		R[3] = AND(AND(D[1], D[2]), NOT(D[0])); //011
		R[4] = AND(AND(NOT(D[1]), NOT(D[2])), D[0]); //100
		R[5] = AND(AND(D[0], D[2]), NOT(D[1])); //101
		R[6] = AND(AND(D[0], D[1]), NOT(D[2])); //110
		R[7] = AND(AND(D[0], D[1]), D[2]); //111
}

//----------- SHIFT ---------

void SHIFTER(bool A[8], bool S[8],bool c)
{
	S[0] = AND(NOT(c),A[1]);
	S[1] = OR(AND(c,A[0]),AND(NOT(c),A[2]));
	S[2] = OR(AND(c,A[1]),AND(NOT(c),A[3]));
	S[3] = OR(AND(c,A[2]),AND(NOT(c),A[4]));
	S[4] = OR(AND(c,A[3]),AND(NOT(c),A[5]));
	S[5] = OR(AND(c,A[4]),AND(NOT(c),A[6]));
	S[6] = OR(AND(c,A[5]),AND(NOT(c),A[7]));
	S[7] = AND(c,A[6]);
}

//----- PALYGINIMAS ----------

bool Palyginimas(bool A[8], bool B[8])
{
bool k1, k2, F, Cflag;
bool rez[8]={0,0,0,0,0,0,0,0};
	std::cout << "001- palyginimas \n";
	for (int i = 0; i < 8; i++) //mux 0,0
		ONEBITALU(A[i], B[i], 1, 1, 0, 0, 0, Cflag, rez[i]);
	k1 = OR(OR(rez[0], rez[1]), OR(rez[2], rez[3]));
	k2 = OR(OR(rez[4], rez[5]), OR(rez[6], rez[7]));
	F = NOR(k1, k2);
	return F;
}

//----- MULTIPLY -------------

void Multiply(bool A[8], bool B[8], bool Output[8], bool& Cflag)
{
bool carryOut=0, carryIn;
for (int i = 7;i >= 0;i--)
{
	bool temp[8]={0,0,0,0,0,0,0,0};
	if (Palyginimas(B, temp)) break;
	else
	{
		if (B[7]==1)
		{
			carryIn=0;
			for (int k = 7;k >= 0;k--)
			{
				ONEBITALU(A[k],B[k],1,1,1,1,carryIn,carryOut,Output[k]);
				carryIn=carryOut;
			}
		}
		SHIFTER(A, Output, 0); //A<<1
		for (int i = 0;i < 8;i++)
			A[i] = Output[i];
		SHIFTER(B, Output, 1); //B>>1
		for (int i = 0;i < 8;i++)
			B[i] = Output[i];
	}
}

}

// ------------------------
// ----- 8 BITU ALU -------
// ------------------------

void EIGHTBITALU(bool A[8], bool B[8], bool D[3], bool Cntr, bool Output[8], bool& Cflag, bool& F)
{
	bool R[8];
	decoder3to8(D, R);
	if (R[0]) //nera operacijos 000
	{
		std::cout << "000 - nera op \n";
	}

	if (R[1]) //A == B 001
	{
		F = Palyginimas(A,B);
	}

	if (R[2]) //nera operacijos 010
	{
		std::cout << "010 - nera op \n";
	}

	if (R[3]) //A-B 011
	{
		// inversija B
		for (int i = 0; i < 8; i++)
			ONEBITALU(0, B[i], 0, 1, 0, 1, 0, Cflag, B[i]);

		// iversija B iki pirmo 1 po apvertimo - ONEBITALU
		for (int i = 7;i >= 0;i--)
		{
			ONEBITALU(0, B[i], 0, 1, 0, 1, 0, Cflag, B[i]);
			if(B[i]) break;
		}

			// A + B - ONEBITALU and
		bool CarryIn = 0, Cflag = 0;
		for (int i = 7;i >= 0;i--)
		{
			ONEBITALU(A[i], B[i], 1, 1, 1, 1, CarryIn, Cflag, Output[i]);
			CarryIn = Cflag;
		}
	}

	if (R[4]) //nera operacijos 100
	{
		std::cout << "100 - nera op \n";
	}

	if (R[5]) //A>>1 A<<1 101
	{
		SHIFTER(A, Output, Cntr);
	}

	if (R[6]) //A+B 110
	{
	bool CarryIn=0;
		for (int i = 7;i >= 0;i--)
			{
			ONEBITALU(A[i], B[i], 1, 1, 1, 1, CarryIn, Cflag, Output[i]);
			CarryIn=Cflag;
			}
	}

	if (R[7]) //A*B 111
	{	
		Multiply(A,B,Output,Cflag);
	}

}

// ------------------------
// ----- MAIN -------------
// ------------------------

int main() {

	bool A[8],B[8],D[3],Output[8]={0,0,0,0,0,0,0,0};
	bool Cntr = 0, Cflag, F = true;

	
	std::cout<<"A: ";
	for (int i = 0;i < 8;i++) std::cin >> A[i];
	std::cout<<'\n'<<"B: ";
	for (int i = 0;i < 8;i++) std::cin >> B[i];
	std::cout << '\n' << "D: ";
	for (int i = 0;i < 3;i++) std::cin >> D[i];
	std::cout << '\n' << "Cntr: ";
	std::cin>>Cntr;

	EIGHTBITALU(A,B,D,Cntr,Output,Cflag,F);
	std::cout<<F<<'\n';
	std::cout<<"Output: ";
	for(int i = 0; i < 8; i++)
	std::cout<<Output[i]<<" ";
	std::cout<<'\n'<<"Carry Flag: "<<Cflag<<'\n';


	/*

	std::cout << " NAND teisingumo lentele:" << "\n";
	std::cout << "-----------------\n";
	std::cout << " A  B | Output \n";
	std::cout << "-----------------\n";

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			std::cout << " " << i << "  " << j << " | " << NAND(i, j) << "\n";
	std::cout << "\n";


	std::cout << " OR teisingumo lentele:" << "\n";
	std::cout << "-----------------\n";
	std::cout << " A  B | Output \n";
	std::cout << "-----------------\n";

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			std::cout << " " << i << "  " << j << " | " << OR(i, j) << "\n";
	std::cout << "\n";

	std::cout << " AND teisingumo lentele:" << "\n";
	std::cout << "-----------------\n";
	std::cout << " A  B | Output \n";
	std::cout << "-----------------\n";

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			std::cout << " " << i << "  " << j << " | " << AND(i, j) << "\n";
	std::cout << "\n";

	std::cout << " NOT teisingumo lentele:" << "\n";
	std::cout << "-----------------\n";
	std::cout << " A  | Output \n";
	std::cout << "-----------------\n";

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			std::cout << " " << i << " | " << NOT(i) << "\n";
	std::cout << "\n";

	std::cout << " XOR teisingumo lentele:" << "\n";
	std::cout << "-----------------\n";
	std::cout << " A  B | Output \n";
	std::cout << "-----------------\n";

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			std::cout << " " << i << "  " << j << " | " << XOR(i, j) << "\n";
	std::cout << "\n";

	std::cout << " NOR teisingumo lentele:" << "\n";
	std::cout << "-----------------\n";
	std::cout << " A  B | Output \n";
	std::cout << "-----------------\n";

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			std::cout << " " << i << "  " << j << " | " << NOR(i, j) << "\n";
	std::cout << "\n";

	std::cout << " SUMOS teisingumo lentele:" << "\n";
	std::cout << "-----------------\n";
	std::cout << " A  B | Output \n";
	std::cout << "-----------------\n";

	bool sum, cout;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int z = 0; z < 2; z++)
			{
				FULLADDER(i, j, z, sum, cout);
				std::cout << " " << i << "  " << j << " | " << sum << cout << "\n";
			}

*/
	return 0;
}