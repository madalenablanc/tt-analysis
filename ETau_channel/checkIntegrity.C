void checkIntegrity(const char* filename) {
    TFile* f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }
    std::cout << "Opened file successfully: " << filename << "\n";
    TIter next(f->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)next())) {
        TObject* obj = key->ReadObj();
        if (!obj) {
            std::cout << "Bad object: " << key->GetName() << "\n";
            continue;
        }
        std::cout << "OK: " << key->GetName() << " (" << obj->ClassName() << ")\n";
        delete obj;
    }
    f->Close();
}
