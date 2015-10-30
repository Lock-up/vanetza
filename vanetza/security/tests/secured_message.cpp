#include <gtest/gtest.h>
#include <vanetza/security/secured_message.hpp>
#include <vanetza/security/tests/check_payload.hpp>
#include <vanetza/security/tests/set_elements.hpp>
#include <vanetza/security/tests/test_elements.hpp>
#include <vanetza/security/tests/web_validator.hpp>

using namespace vanetza::security;

TEST(SecuredMessage, Serialization)
{
    SecuredMessage m;

    m.header_fields = setHeaderField_list();
    m.payload = {PayloadType::Unsecured, { 5, 10, 15, 25, 40 }};
    std::list<TrailerField> list;
    list.push_back(setSignature_Ecdsa_Signature());
    list.push_back(setSignature_Ecdsa_Signature());
    m.trailer_fields = list;

    std::stringstream stream;
    OutputArchive oa(stream);
    serialize(oa, m);

    SecuredMessage deMessage;
    InputArchive ia(stream);
    deserialize(ia, deMessage);

    testHeaderFieldList(m.header_fields, deMessage.header_fields);
    check(m.payload, deMessage.payload);
}

TEST(SecuredMessage, WebValidator_Serialize_SecuredMessageV2_1)
{
    // SecuredMessage/v1 from FOKUS WebValidator adapted for v2
    const char str[] =
        "0281038002010901A8ED6DF65B0E6D6A0100809400000418929DB6A9E452223062C52028E956BF9874E0A40D21D5F9F56564F39C5DD187C922F2E5F0630373879A43393373B9F6205BF01FBD9C1F113165C291C376F535010004EABA91A915D81807E910FD292D99DF8B401EED88CF7F031412D5ED9905F9996469798C412FC8F7237A3AB3469795E2DEF5E1B783EA4F6B6A2359D21772B2EA9D0200210AC040800101C0408101010F01099EB20109B1270003040100960000004B2E6D0D0EE9BC4AD9CD087B601E9AF06031995443D652763455FBB794B33982889260740EF64CFA8C6808A58F98E06CE42A1E9C22A0785D7242647F7895ABFC0000009373931CD758050002011C983E690E5F6D755BD4871578A9427E7BC383903DC7DA3B560384013643010000FE8566BEA87B39E6411F80226E792D6E01E77B598F2BB1FCE7F2DD441185C07CEF0573FBFB9876B99FE811486F6F5D499E6114FC0724A67F8D71D2A897A7EB34";
    std::stringstream stream;
    stream_from_string(stream, str);
    SecuredMessage m;
    InputArchive inAr(stream);
    deserialize(inAr, m);

    EXPECT_EQ(2, m.protocol_version());

    ASSERT_EQ(3, m.header_fields.size());
    ASSERT_EQ(HeaderFieldType::Signer_Info, get_type(m.header_fields.front()));
    auto signer_info = boost::get<SignerInfo>(m.header_fields.front());
    m.header_fields.pop_front();
    EXPECT_EQ(HeaderFieldType::Generation_Time, get_type(m.header_fields.front()));
    m.header_fields.pop_front();
    ASSERT_EQ(HeaderFieldType::Message_Type, get_type(m.header_fields.front()));
    auto message_type = boost::get<uint16_t>(m.header_fields.front());
    EXPECT_EQ(2, message_type);

    EXPECT_EQ(PayloadType::Signed, m.payload.type);
    EXPECT_EQ(28, m.payload.buffer.size());

    ASSERT_EQ(1, m.trailer_fields.size());
    EXPECT_EQ(TrailerFieldType::Signature, get_type(m.trailer_fields.front()));
}

TEST(SecuredMessage, WebValidator_Serialize_SecuredMessageV2_2)
{
    // SecuredMessage/v1 from FOKUS WebValidator adapted for v2
    const char str[] =
        "0281038002010901A8ED6DF65B0E6D6A01008094000004C4EC137145DD4F450145DE530CCA36E73AB3D87FC8275847CDAD8248C1CD20879BD6A8CB54EA9E05D3B41376CE2F24789AEF82836CA818D568ADF4A140E96E48010004D6C268EE68B5B8B387B2312B7E1D21CE0C366D251A32431508B96EB6A3479CCF96A8738F30ED451F00DA8DDE84367C7EB16727D14FF14F5DD8F9791FE0A12A640200210AC040800101C0408101010F01099EB20109B1270003040100960000001EB035FE8E51DCDD8558DE0BE9B87895B36B420583A5C6B2B8B2EAB7F3D3C99163638FA025A0033D4BD80BBA02B8E3DE1B55766459D494677AF24917E51B80AC0000009373CC5F22C80500020120F29384759027349075829034707ABABABABABAABAB98437985739845783974954301000081E7CDB6D2C741C1700822305C39E8E809622AF9FCA1C0786F762D08E80580C42F1FCC1D5499577210834C390BB4613E102DECB14F575A2820743DC9A66BBD7A";
    std::stringstream stream;
    stream_from_string(stream, str);
    InputArchive inAr(stream);
    SecuredMessage m;
    deserialize(inAr, m);
}

TEST(SecuredMessage, WebValidator_Serialize_SecuredMessageV2_3)
{
    // SecuredMessage/v1 from FOKUS WebValidator adapted for v2
    const char str[] =
        "0281038002010901A8ED6DF65B0E6D6A010080940000040209B0434163CCBAFDD34A45333E418FB96C05BBE0E7E1D755D40D0B4BBE8DA508EC2F2723B7ADF0F27C39F3AECFF0783C196F9961F8821E6294375D9294CD6A01000452113CE698DB081491675DF8FFE81C23EA5D0071B2D2BF0E0DA4ADA0CDA58259CA5D999200B6565E194EDAB8BD3DCA863F2DDF39C13E7A0375ECE2566C5EB8C60200210AC040800101C0408101010F01099EB20109B1270003040100960000008DA1F3F9F35E04C3DE77D7438988A8D57EBE44DAA021A4269E297C177C9CFE458E128EC290785D6631961625020943B6D87DAA54919A98F7865709929A7C6E480000009373CF482D4005000201080123456789ABCDEF43010000371423BBA0902D8AF2FB2226D73A7781D4D6B6772650A8BEE5A1AF198CEDABA2C9BF57540C629E6A1E629B8812AEBDDDBCAF472F6586F16C14B3DEFBE9B6ADB2";
    std::stringstream stream;
    stream_from_string(stream, str);
    InputArchive inAr(stream);
    SecuredMessage m;
    deserialize(inAr, m);
}

TEST(SecuredMessage, WebValidator_Serialize_Certificate_RootCA)
{
    char str[] =
        "010100040D50696C6F74504B495F526F6F74808900000410C1B6C130E84711B558E69CE0FBC17DE6B9A6C42BB7E1B962EF4CD5B07DADF8D214C65D8B7BD24ACCE01EDFE5F186A94217DE25E454763F5D83A1161860D6FC010100049B7D10065669DFE403CEB43E2A57B8061C86B9B8A0ADF2ECE2FEFD5A09872525B89F1D8732A4D761E68446C23FB845CD5C8172D3067B9C61CDEFBD6923C1FBC002E00B0105A4EC0110EC21010300000008610502F11FC54CA9D198DE0177DD571F12E5945A771E20CEDA080A00FBFC0D00B0B38F6AC60C18F2CD79D90C2AFE45BD76349E0454817E184F92BF87D7C9B4";
    std::stringstream stream;
    stream_from_string(stream, str);
    InputArchive inAr(stream);
    Certificate c;
    deserialize(inAr, c);
}

TEST(SecuredMessage, WebValidator_Serialize_Certificate_EnrollmentAuthority)
{
    char str[] =
        "0180EC02010100040D50696C6F74504B495F526F6F74808900000410C1B6C130E84711B558E69CE0FBC17DE6B9A6C42BB7E1B962EF4CD5B07DADF8D214C65D8B7BD24ACCE01EDFE5F186A94217DE25E454763F5D83A1161860D6FC010100049B7D10065669DFE403CEB43E2A57B8061C86B9B8A0ADF2ECE2FEFD5A09872525B89F1D8732A4D761E68446C23FB845CD5C8172D3067B9C61CDEFBD6923C1FBC002E00B0105A4EC0110EC21010300000008610502F11FC54CA9D198DE0177DD571F12E5945A771E20CEDA080A00FBFC0D00B0B38F6AC60C18F2CD79D90C2AFE45BD76349E0454817E184F92BF87D7C9B4030D50696C6F74504B495F4C5443418089000004952AD9BB6A2DF1D21E252FB87EDA8160CD4B6F671D8154BC6FA164A879E5B54DD7D92EECABBD882C0E30BD855757F5B51C888890AADE9CBD9859F729FB6BCDA10101000400D72336027966C93C815C548F279E0D226AD39F5D8634D5A1B64541E1A87F025C3E790B1394DD0CCD9898C0C0AC51F56B2D265AF118E64953C4B2BA64324AAD02E00B0105A4EC0110EC210103000000968ACF98D59BADC6989D38EF2BBE767D0F075F34333F158FE73C98876AD67794AD47A557AB3BAE315BD7E8719FAE36B3FE20B4AB4DE5FC5A21E84A3989E273C6";
    std::stringstream stream;
    stream_from_string(stream, str);
    InputArchive inAr(stream);
    Certificate c;
    deserialize(inAr, c);
}

TEST(SecuredMessage, WebValidator_Serialize_Certificate_AuthorizationAuthority1)
{
    char str[] =
        "01090183CA2168B784FC6C021050696C6F74504B495F5043415F53495480910000040D4661BF77F671716EA74EA4CD92EF5A4566BEEA6681ED044B09C31EA47D42B388932A9B2DA957F3FD2E1256DAC8069B1544BF2988D43EF09B8AABC67813465E010100043361E73DFF9BC89C42F4542AAC7FDD9A5ABA66531087D1C31C5A95B6788E06BC08F9402D3ED4756A6FB06937F2A919833841358D052FB6C9AF4CC1F684C3B76A02E02006C04080C040810B0105A4EC0110EC210103000000C64E89A23528F50D8AF8536290FE9E78CC26F51D6B4278EC086383A4AFEABA961AD38A2FB286D0F075DEDB0C7929960D35865E80A3A022CAC772F1DACA6F875A";
    std::stringstream stream;
    stream_from_string(stream, str);
    InputArchive inAr(stream);
    Certificate c;
    deserialize(inAr, c);
}

TEST(SecuredMessage, WebValidator_Serialize_Certificate_AuthorizationAuthority2)
{
    char str[] =
        "01090183CA2168B784FC6C021050696C6F74504B495F5043415F5349548089000004732CA0163B0E3CD6DE8789FED98DE772F3D54EBFF1D44ED556B683B0D8AE837C8B1EB52F25377244298655CDCB3D3CCEA8CEDA1AA5E57920C97FEC38F2237B0B0101000412FD16EA68D73575651CD7AED08023CFDC38AEE3CCAA178D672AB30A6FE7803CAEFCF1F79A373C7D77D84AE2DD079229D50E5745CD73398FB69D902617A6AAB602E00B0105A4EC0110EC210103000000A9A8140518E0C66DED7568E103C9D3927CF83B6D4692B7FA013BE586FB6BA7A17F0DD28FE14317E2B9F141084166F24F0CBD06AB926197F5F6A39C6003640E64";
    std::stringstream stream;
    stream_from_string(stream, str);
    InputArchive inAr(stream);
    Certificate c;
    deserialize(inAr, c);
}

TEST(SecuredMessage, WebValidator_Serialize_Certificate_AuthorizationAuthority3)
{
    char str[] =
        "0180EC02010100040D50696C6F74504B495F526F6F74808900000410C1B6C130E84711B558E69CE0FBC17DE6B9A6C42BB7E1B962EF4CD5B07DADF8D214C65D8B7BD24ACCE01EDFE5F186A94217DE25E454763F5D83A1161860D6FC010100049B7D10065669DFE403CEB43E2A57B8061C86B9B8A0ADF2ECE2FEFD5A09872525B89F1D8732A4D761E68446C23FB845CD5C8172D3067B9C61CDEFBD6923C1FBC002E00B0105A4EC0110EC21010300000008610502F11FC54CA9D198DE0177DD571F12E5945A771E20CEDA080A00FBFC0D00B0B38F6AC60C18F2CD79D90C2AFE45BD76349E0454817E184F92BF87D7C9B4021050696C6F74504B495F5043415F5349548089000004B25F258C4E10D460F62F11D1F54BEACFF521E7F4758EE73A8623B23536A562EA915F470098485FC1E3CC6344FD741E5AF491CCE8E9D7F47411E607B75B8F7899010100047B9C36689AD2C056DFF9027903558959B28C0830B98C2303D7EDF61EE466122270709E833B71C012C248A742669212EBD2E355881F7AC1192A782D2D3D1BB40702E00B0105A4EC0110EC2101030000002F04BDFC9342276F28BBA11B771F9530E910D1B00ADA4B1EC02C6D591F9EED7652F683DAEFC0C73CDD2D5A243CE64DA46BC952493A29320782B308B051F8492D";
    std::stringstream stream;
    stream_from_string(stream, str);
    InputArchive inAr(stream);
    Certificate c;
    deserialize(inAr, c);
}

