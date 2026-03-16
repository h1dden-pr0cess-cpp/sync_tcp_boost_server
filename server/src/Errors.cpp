#include "Errors.hpp"


Packet Errors::send_error(ErrorCode code)
{
    std::vector<uint8_t> error;
    write_error(error, code);
    Packet response;
    response.type = PacketType::Error; 
	response.body = error;
    return response;
}

void Errors::write_error(std::vector<uint8_t>& error,
                         ErrorCode error_type)
{
    
switch(static_cast<ErrorCode>(error_type)) {
        case ErrorCode::InvalidPacket:
        {
            convert_error(error, "Packet error");
            return;
        }
        case ErrorCode::InvalidToken:
        {
            convert_error(error, "Token error"); 
            return;
        }
        case ErrorCode::UserAlreadyExist:
        {
            convert_error(error, "User already exist");
        }
        case ErrorCode::InvalidLogin:
        {
            convert_error(error, "Invalid login");
            return;
        }
        case ErrorCode::GameNotFound:
        {
            convert_error(error, "Game not found");
            return;
        }
        case ErrorCode::CannotAddGame:
        {
            convert_error(error, "Cannot add game");
        }
		case ErrorCode::SaveNotFound: 
        {
            convert_error(error, "Save not found");
			return;
		}
        case ErrorCode::CannotAddSave:
        {
            convert_error(error, "Cannot add save");
            return;
        }
        case ErrorCode::UnknownPacket:
        {
            convert_error(error, "Unknown packet");
            return;
        }
            deafault:
        {
            convert_error(error, "Unknown error");
            return;
        }
}		
}

void Errors::convert_error(std::vector<uint8_t>& error, 
                   const std::string error_str)
{
        error.insert(
        error.end(),
        reinterpret_cast<const uint8_t*>(error_str.data()),
        reinterpret_cast<const uint8_t*>(error_str.data()) + error_str.size());
}
