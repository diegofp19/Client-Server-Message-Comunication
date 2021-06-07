
struct username {
    opaque username[256];
};


program COMUNICACION
{
    version COMUNICACIONVER
    {

        int add_register_rpc(username reg) = 1;

        int delete_register_rpc(username reg) = 2;

        int existRegister_rpc(username reg) = 3;

        
    }
    = 1;
}
= 1;


