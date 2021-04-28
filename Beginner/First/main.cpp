#include <json/json.h>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include "sha256.h"

using namespace std;

class StringUtil
{
public:
	// Applies Sha256 to a string and returns the result
	static string applySha256(string input)
	{
		return sha256(input);
	}
};

class Block
{
private:
	string data; // our data will be a simple message.
	long long timeStamp; // as number of milliseconds since 1/1/1970
	int nonce;

public:
	string hash;
	string previousHash;

	// Block constructor
	Block(string _data, string _previousHash)
	{
		this->data = _data;
		this->previousHash = _previousHash;
		chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(
			chrono::system_clock::now().time_since_epoch());
		this->timeStamp = ms.count();
		this->nonce = 0;
		this->hash = calculateHash();
	}

	Block() {
		this->data = "";
		this->timeStamp = 0;
		this->hash = "";
		this->previousHash = "";
		this->nonce = 0;
	}

	~Block() {}
	Block operator=(const Block& b)
	{
		this->data = b.data;
		this->timeStamp = b.timeStamp;
		this->hash = b.hash;
		this->nonce = b.nonce;
		this->previousHash = b.previousHash;
		return *this;
	}

	string calculateHash()
	{
		StringUtil string_util;
		string calculate_hash = string_util.applySha256(
			this->previousHash +
			to_string(this->timeStamp) +
			to_string(this->nonce) +
			this->data
		);

		// hash = calculate_hash;

		return calculate_hash;
	}

	friend ostream& operator<<(ostream& os, Block p)
	{
		os << "\t{" << endl;
		os << "\t\t\"hash\": \"" << p.hash << "\"," << endl;
		os << "\t\t\"previousHash\": \"" << p.previousHash << "\"," << endl;
		os << "\t\t\"data\": \"" << p.data << "\"," << endl;
		os << "\t\t\"timeStamp\": " << p.timeStamp << "," << endl;
		os << "\t\t\"nonce\": " << p.nonce << endl;
		os << "\t}," << endl;
		return os;
	}

	void mineBlock(int difficulty)
	{
		string target = "";
		for (int i = 0; i < difficulty; i++)
			target += "0";

		while (this->hash.substr(0, difficulty) != target)
		{
			nonce++;
			this->hash = calculateHash();
		}
		cout << "Block Mined!!! : " << this->hash << endl;
	}
};

class NoobChain
{
private:
	vector<Block>block_chain;
	int difficulty = 6;
public:
	void _main_()
	{
		// add our blocks to the blockchain
		block_chain.push_back(Block("Hi im the first blocks data.", "0"));
		cout << "Trying to Mine block 1... " << endl;
		block_chain.back().mineBlock(difficulty);

		block_chain.push_back(Block("Yo im the second block", block_chain.back().hash));
		cout << "Trying to Mine block 2... " << endl;
		block_chain.back().mineBlock(difficulty);

		block_chain.push_back(Block("Hey im the third block", block_chain.back().hash));
		cout << "Trying to Mine block 3... " << endl;
		block_chain.back().mineBlock(difficulty);

		cout << "Blockchain is Valid: " << this->isChainValid() << endl;
	}

	bool isChainValid()
	{
		Block currentBlock;
		Block previousBlock;
		string hashTarget = "";
		for (int i = 0; i < this->difficulty; i++)
			hashTarget += "0";

		int n = this->block_chain.size();
		for (int i = 1; i < n; i++)
		{
			currentBlock = block_chain[i];
			previousBlock = block_chain[i - 1];

			// compare registered hash and calculated hash
			if (currentBlock.hash != currentBlock.calculateHash())
			{
				cout << "Current Hashes not equal" << endl;
				return false;
			}

			// compare previous hash and registered previous hash
			if (previousBlock.hash != currentBlock.previousHash)
			{
				cout << "Previous Hashes not equal" << endl;
				return false;
			}

			// check if hash is solved
			if (currentBlock.hash.substr(0, difficulty) != hashTarget)
			{
				cout << "This block hasn't been mined" << endl;
				return false;
			}
		}
		return true;
	}

	void print()
	{
		int n = this->block_chain.size();

		cout << "[" << endl;
		for (int i = 0; i < n; i++)
		{
			cout << this->block_chain[i];
		}
		cout << "]" << endl;
	}
};

class Wallet
{
public:
	string privateKey;
	string publicKey;

	string  gen_random(int len) {
		string s;
		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";

		for (int i = 0; i < len; ++i) {
			s += alphanum[rand() % (sizeof(alphanum) - 1)];
		}

		return s;
	}

	void generateKeyPair()
	{
		privateKey = gen_random(256);
		publicKey = gen_random(256);

		cout << privateKey << endl;
		cout << publicKey << endl;
		cout << (privateKey == publicKey) << endl;
	}
};

class TransactionOutput
{
public:
	string id;
	string publicKeyReciepient; // also known as the new owner of these coins.
	float value; // the amount of coins they own
	string parentTransactionId; // the id of the transaction this output was created in

	//Constructor
	TransactionOutput(string _public_key_reciepient, float _value, string _parent_transaction_id)
	{
		this->publicKeyReciepient = _public_key_reciepient;
		this->value = _value;
		this->parentTransactionId = _parent_transaction_id;
		this->id = StringUtil::applySha256(
			this->publicKeyReciepient +
			to_string(this->value) +
			this->parentTransactionId
		);
	}

	// Check if coin belongs to you
	bool isMine(string publicKey)
	{
		return (publicKey == this->publicKeyReciepient);
	}
};

class TransactionInput
{
public:
	string transactionOutputId; // Reference to TransactionOutputs -> transactionId
	TransactionOutput UTXO; //Contains the Unspent transaction output

	TransactionInput(string transactionOutputId) {
		this->transactionOutputId = transactionOutputId;
	}
};

class Transaction
{
public:
	string transactionId; // this is also the hash of the transaction.
	string publicKeySender;
	string publicKeyReciepient;
	float value;
	string signature;

	vector<TransactionInput>inputs;
	vector<TransactionOutput>outputs;

	// Constructor: 
	Transaction(string _public_key_from, string _public_key_to, float _value, vector<TransactionInput>_inputs)
	{
		this->publicKeySender = _public_key_from;
		this->publicKeyReciepient = _public_key_to;
		this->value = _value;
		this->inputs = _inputs;
	}

private:
	int sequence = 0;

	// This Calculates the transaction hash (which will be used as its Id)
	string calulateHash()
	{
		sequence++; //increase the sequence to avoid 2 identical transactions having the same hash
		return StringUtil::applySha256(
			this->publicKeySender +
			this->publicKeyReciepient +
			to_string(value) +
			to_string(sequence)
		);
	}
};



int main(int argc, char* argv[])
{
	/*NoobChain noob_chain;
	noob_chain._main_();

	cout << endl << "The Block Chain:" << endl;
	noob_chain.print();*/

	Wallet w;
	w.generateKeyPair();

	return 0;
}
