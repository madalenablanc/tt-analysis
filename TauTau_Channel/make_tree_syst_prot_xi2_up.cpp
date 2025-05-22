#include <iostream>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

int main() {
    // Abrir o arquivo de entrada
    TFile* inputFile = TFile::Open("TauTau_sinal_PIC_march_2018.root", "READ");
    if (!inputFile) {
        std::cerr << "Erro ao abrir o arquivo de entrada." << std::endl;
        return 1;
    }

    // Ler a árvore do arquivo de entrada
    TTree* inputTree = dynamic_cast<TTree*>(inputFile->Get("tree"));
    if (!inputTree) {
        std::cerr << "Erro ao obter a árvore do arquivo de entrada." << std::endl;
        inputFile->Close();
        return 1;
    }

    // Criar um novo arquivo de saída
    TFile* outputFile = TFile::Open("TauTau_sinal_PIC_march_2018_xi2_up_syst.root", "RECREATE");
    if (!outputFile) {
        std::cerr << "Erro ao criar o arquivo de saída." << std::endl;
        inputFile->Close();
        return 1;
    }

    // Criar uma nova árvore de saída
    TTree* outputTree = inputTree->CloneTree(0); // Clone a estrutura da árvore, sem copiar os eventos

    // Criar as variáveis para armazenar os valores das folhas do inputTree
    Double_t xi_arm2_1;
    Double_t xi_arm2_1_up;

    // Criar os apontadores para as folhas do inputTree
    TBranch* b_xi_arm2_1 = nullptr;
    TBranch* b_xi_arm2_1_up = nullptr;

    // Associar os apontadores às folhas do inputTree
    inputTree->SetBranchAddress("xi_arm2_1", &xi_arm2_1, &b_xi_arm2_1);
    inputTree->SetBranchAddress("xi_arm2_1_up", &xi_arm2_1_up, &b_xi_arm2_1_up);

    // Obter o número de eventos na árvore
    Long64_t numEntries = inputTree->GetEntries();

    // Copiar as folhas do inputTree para o outputTree
    for (Long64_t i = 0; i < numEntries; ++i) {
        // Obter as entradas das folhas do inputTree para o evento atual
        inputTree->GetEntry(i);

        // Atualizar o valor de xi_arm1_1 com xi_arm1_1_up
        xi_arm2_1 = xi_arm2_1_up;

        // Preencher as folhas atualizadas no outputTree
        outputTree->Fill();
    }

    // Escrever a árvore de saída no arquivo
    outputTree->Write();

    // Fechar os arquivos
    outputFile->Close();
    inputFile->Close();

    std::cout << "Processamento concluído." << std::endl;

    return 0;
}

