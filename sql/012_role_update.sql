CREATE OR REPLACE FUNCTION prevent_PO_update()
RETURNS TRIGGER
AS $$
BEGIN
    IF NEW.role = 'PO' OR OLD.role = 'PO' THEN
        RAISE EXCEPTION 'Cannot update PO';
    END IF;
    RETURN NEW;
END $$
language plpgsql;


CREATE TRIGGER trg_prevent_PO_update
BEFORE UPDATE ON Member
FOR EACH ROW
EXECUTE FUNCTION prevent_PO_update();